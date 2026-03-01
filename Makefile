install:
	install -Dm644 bashrc ~/.bashrc
	install -Dm644 bash_profile ~/.bash_profile
	install -Dm644 tmux.conf ~/.tmux.conf
	install -Dm644 vimrc ~/.vimrc
	install -Dm644 ctags ~/.ctags
	install -Dm644 mutt/muttrc ~/.mutt/muttrc
	install -Dm644 mbsyncrc ~/.mbsyncrc

clean:
	rm -f ~/.bashrc ~/.bash_profile ~/.tmux.conf ~/.vimrc
