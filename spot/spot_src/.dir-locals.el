((nil . ((whitespace-action auto-cleanup)
	 (whitespace-style face empty trailing lines-tail)
	 (require-final-newline . t)
	 (mode . global-whitespace)
         (bug-reference-bug-regexp
          . "\\(?:[Ff]ix\\(es\\)? \\|[Ii]ssue \\)#\\(?2:[0-9]+\\)")
         (bug-reference-url-format
          . "https://gitlab.lrde.epita.fr/spot/spot/issues/%s")
	 (mode . bug-reference)
         (magit-branch-adjust-remote-upstream-alist ("origin/next" . "/"))))
 (c++-mode . ((c-default-style . "gnu")
	      (indent-tabs-mode . nil)
	      (tab-width . 8)
	      (fill-column . 70)))
 ("tests" . ((nil . ((eval . (defun spot/execute-test ()
                               "execute the current file through ../run"
                               (interactive)
                               (shell-command (concat "cd ..; ./run "
                                                      (buffer-file-name)
                                                      " &"))))))
             (sh-mode . ((eval . (local-set-key
                                  (kbd "C-c C-c") #'spot/execute-test))))
             (python-mode . ((eval . (local-set-key (kbd "C-c C-c")
                                                    #'spot/execute-test)))))))
