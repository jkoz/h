install:
	install -Dm644 bashrc ~/.bashrc
	install -Dm644 bash_profile ~/.bash_profile
	install -Dm644 tmux.conf ~/.tmux.conf
	install -Dm644 vimrc ~/.vimrc

clean:
	rm -f ~/.bashrc ~/.bash_profile ~/.tmux.conf ~/.vimrc
