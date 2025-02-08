install:
	install -Dm644 bashrc ~/.bashrc
	install -Dm644 tmux.conf ~/.tmux.conf
	install -Dm644 vimrc ~/.vimrc

clean
	rm -f ~/.bashrc ~/.tmux.conf ~/.vimrc	
