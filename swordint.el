;;; sword.el --- Calls sword library through irenaeus

;; Author: Richard Holcombe <raholcom@eos.ncsu.edu>

;; This file is part of GNU Emacs.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;; Code:



;; (autoload 'sword-mode "sword" "Enter sword mode." t)

(require 'easymenu)
(require 'tempo)
(require 'derived)

(defconst interactive-sword-mode-version "0.0.1"
  "Current version of interactive sword mode.")


(defun sword-show-version ()
  "Show the version number in the minibuffer."
  (interactive)
  (message "sword-mode, version %s" sword-mode-version))


(defvar sword-text-module nil
  "Default biblical text.")
(defvar sword-ld-module nil
  "Default biblical text.")
(defvar sword-comm-module nil
  "Default biblical text.")

;;; XXX should do search to choose a module

  (setq sword-text-module "KJV")


(defun sword-set-module (str)
  "Set the default module to use"
  (interactive "sWhich module:")
  (setq sword-text-module str)
  )


(defun sword-search-module-string (modname str)
"Search bible for specified text string.  Specify module then search string"
(interactive "sModule name:
sString to search for:")
  (let ((command-args (list "-c" "M" modname "s" (concat "[" str "]") )))
    (setq program "irenaeus")
    (setq buffer-read-only nil)
    (apply 'call-process program nil t nil command-args)))


(defun sword-search (str)
"Search bible for specified text string.  Specify module then search string"
(interactive "sString to search for:")
(sword-search-module-string sword-text-module str ))


(defun sword-lookup-module-string (modname str)
"Looks up a word in a lexicon or dictionary.  Specify module then ref text."
(interactive "sModule name:
stext to lookup:")
  (let (
	(command-args (list "-c" "M" modname "l" (concat "[" str "]"))))
    (setq program "irenaeus")
    (setq buffer-read-only nil)
    (apply 'call-process program nil t nil command-args)))


(defun sword-lookup (str)
"Looks up a word in a lexicon or dictionary"
(interactive "sString to lookup:")
(sword-lookup-module-string sword-text-module str))


(defun sword-get-all-module-list ()
"gets list of modules of type str=1->Biblical Text, 2->Commentaries, 3->lexicon or dictionary"
(interactive)
  (let ((command-args (list "-c" "m")))
    (setq program "irenaeus")
    (setq buffer-read-only nil)
    (apply 'call-process program nil t nil command-args)))


(defun sword-get-module-list (str)
"gets list of modules of type str=1->Biblical Text, 2->Commentaries, 3->lexicon or dictionary"
(interactive "sModule type number")
  (let ((command-args (list "-c" "R" str)))
    (setq program "irenaeus")
    (setq buffer-read-only nil)
    (apply 'call-process program nil t nil command-args)))

(defun sword-get-bible-module-list ()
"returns a list of available bible text modules"
(interactive)
(sword-get-module-list "1"))

(defun sword-get-comm-module-list ()
"returns a list of available commentary modules"
(interactive)
(sword-get-module-list "2"))

(defun sword-get-ld-module-list ()
"returns a list of available lexicon/dictionary modules"
(interactive)
(sword-get-module-list "3"))


(defun sword-bibleref (str)
 "looks up and inserts a bible reference"
(interactive "sBiblical reference:")
  (let ((dir default-directory)
	(command-args (list "-c" "l"  (concat "[" str "]")  )))
    (setq program "irenaeus")
    (setq default-directory dir)
    (setq buffer-read-only nil)
    (insert-before-markers
     (with-temp-buffer 
       (insert (apply 'call-process program nil t nil command-args))
       (beginning-of-buffer)
       (replace-regexp "<[A-Za-z0-9]+>" "" )
       (replace-regexp " " "" )
       (buffer-string)))))

(defvar sword-bibletext-list nil)


;;=============================================================================
;; register sword major mode keymap.
;;=============================================================================


;;;  (use-local-map sword-mode-map)
;;;  (setq major-mode 'sword-mode)
;;;  (setq mode-name "Sword")

(defvar sword-mode-menu-keymap nil
  "Keymap used in Sword mode to provide a menu.")

(defun sword-frame-init ()
  "Initialize Emacs 19+ menu system."
  (interactive)
  ;; make a menu keymap
  (easy-menu-define
   sword-mode-menu
   sword-mode-map
   "Sword menu"
   '("Sword"
     ["Lookup verse" sword-lookup t]
     ["Search for string" sword-search t]
     ["Set module " sword-set-module t]
     "----"
     ("List Modules"
      ["list all modules" sword-get-all-module-list t]
      ["list Bible modules" sword-get-bible-module-list t]
      ["list Commentary modules" sword-get-comm-module-list t]
      ["list Lex/Dict modules" sword-get-ld-module-list t]
      )
     ["Version" sword-show-version t]
    ))
  (easy-menu-add sword-mode-menu sword-mode-map))

(defvar sword-mode-map nil
  "Keymap used in Sword mode.")
(if sword-mode-map
    ()
  (let ((map (make-sparse-keymap)))
    (define-key map "\C-cl" 'sword-lookup)
    (define-key map "\C-cs" 'sword-search)
    (setq sword-mode-map map)))


(defvar irenaeus-shell-buffer-name "Ireneaus"
  "Name used to create `interactive-sword-shell' mode buffers.
This name will have *'s surrounding it.")



(defcustom irenaeus-shell-command "irenaeus"
  "*The name of the command to be run which will start the Ireneaus process."
  :group 'irenaeus-shell
  :type 'string)


(defcustom irenaeus-shell-command-switches "-f"
  "*Command line parameters run with `irenaeus-shell-command'."
  :group 'irenaeus-shell
  :type '(string :tag "Switch: "))


(defun irenaeus-shell-active-p ()
  "Return t if the Ireneaus shell is active."
  (if (get-buffer (concat "*" irenaeus-shell-buffer-name "*"))
      (save-excursion
	(set-buffer (concat "*" irenaeus-shell-buffer-name "*"))
	(if (comint-check-proc (current-buffer))
	    (current-buffer)))))


(defun irenaeus-shell-not-running ()
  "Return a running irenaeus buffer iff it is currently active."
  (or (irenaeus-shell-active-p)
      (error "You need to run the command `irenaeus-shell' to do that!")))

(defun irenaeus-shell-redraw-command (command)
  "If there is a irenaeus shell, clear buffer andrun the irenaeus COMMAND and 
return it's output.  It's output is returned as a string with 
no face properties."
  (let ((msbn (irenaeus-shell-not-running))
	(pos nil)
	(str nil)
	(lastcmd))
    (save-excursion
      (set-buffer msbn)
      (setq pos (point))
      (comint-send-string (get-buffer-process (current-buffer))
			  (concat command "\n"))
      (goto-char (point-max)) 
	(accept-process-output (get-buffer-process (current-buffer)))
	(goto-char (point-max))
      (save-excursion
	(goto-char pos)
	(beginning-of-line)
	(setq str (buffer-substring-no-properties (save-excursion
						    (goto-char pos)
						    (beginning-of-line)
						    (forward-line 1)
						    (point))
						  (save-excursion
						    (goto-char (point-max))
						    (beginning-of-line)
						    (point))))
	(delete-region pos (point-max)))
      (insert lastcmd))
    str))

;;;(irenaeus-shell-redraw-command nil)

(defun interactive-sword-mode () 
  "Major mode that uses sword program."
  (interactive)
  (kill-all-local-variables)
  (use-local-map sword-mode-map)
  (setq major-mode 'interactive-sword-mode)
  (setq mode-name "intSword")
  (if window-system (sword-frame-init))
  (erase-buffer)
;;;  (comint-mode)
  (switch-to-buffer
   (make-comint irenaeus-shell-buffer-name irenaeus-shell-command
		nil irenaeus-shell-command-switches))
  )


;;; provide ourself

(provide 'sword)





