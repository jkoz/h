" Vimplug {{{

let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
    silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
    autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif

 call plug#begin('~/.vim/plugged')

Plug 'altercation/vim-colors-solarized'
Plug 'tpope/vim-commentary'
Plug 'tpope/vim-surround'
Plug 'tpope/vim-fugitive'
Plug 'tpope/vim-vinegar'
Plug 'tpope/vim-repeat'
Plug 'tpope/vim-unimpaired'
Plug 'tpope/vim-dispatch'
Plug 'tpope/vim-rsi'
Plug 'tpope/vim-speeddating'

Plug 'mattn/emmet-vim'
Plug 'jkoz/vim-fuzzy'
Plug 'natebosch/vim-lsc'

" Plug 'airblade/vim-rooter'
" Plug 'Yggdroot/indentLine'
" Plug 'yegappan/lsp'
" Plug 'girishji/scope.vim'
" Plug 'girishji/vimsuggest'
" Plug 'masukomi/vim-markdown-folding'
" Plug 'rhysd/git-messenger.vim'
" Plug 'octol/vim-cpp-enhanced-highlight'
" Plug 'easymotion/vim-easymotion'
" Plug 'dhruvasagar/vim-table-mode'


call plug#end()
filetype plugin indent on
syntax on

" }}}

" Tabline {{{
function! MyTabLine()
  let s = ''
  for i in range(tabpagenr('$'))
    let tabnr = i + 1 " range() starts at 0
    let winnr = tabpagewinnr(tabnr)
    let buflist = tabpagebuflist(tabnr)
    let bufnr = buflist[winnr - 1]
    let bufname = fnamemodify(bufname(bufnr), ':t')

    let s .= '%' . tabnr . 'T'
    let s .= (tabnr == tabpagenr() ? '%#TabLineSel#' : '%#TabLine#')
    let s .= ' ' . tabnr

    let n = tabpagewinnr(tabnr,'$')
    if n > 1 | let s .= ':' . n | endif

    let s .= empty(bufname) ? ' [No Name] ' : ' ' . bufname . ' '

    let bufmodified = getbufvar(bufnr, "&mod")
    if bufmodified | let s .= '+ ' | endif
  endfor
  let s .= '%#TabLineFill#'
  return s
endfunction
set tabline=%!MyTabLine()
set showtabline=1 " 2. always show tabline, 1 show when having more than 1 tab
" }}}

" Tabs {{{
nn <silent> tt :tabedit %<CR>
nn <silent> [g :tabprevious<CR>
nn <silent> ]g :tabnext<CR>
nn <silent> [G :tabrewind<CR>
nn <silent> ]G :tablast<CR>
" }}}

" Mappings {{{
nn <space> <nop>
let mapleader = " "

" disable jk as escape because it cause slow down in virtual selection mode.
ino <leader><leader> <esc>
vn <leader><leader> <esc>

" change : to ;
nn ; :
nn : ;

" Toggle wrap
nn <Leader>W :setl nowrap! <CR>

" Case insensitive search use easy motion instead
nn / /\v
vn / /\v

" match bracket pairs with tab is a hell of a lot easier than %
" map <tab> %

" clear search match
nn <silent> <leader>v :nohl<cr>

" Quick quit
nn <silent> q :q<cr>
nn <silent> - :bp\|bd #<cr>

"com! BW :bp|:bd#
"nn <silent> - :BW<cr>


" Quick save
nn <leader>w :update<cr>

" copy full directory path of opened file to clipboard
nmap cpd :let @+ = expand("%:p:h")<CR>
" copy full file path of opened file to clipboard
nmap cpf :let @+ = expand("%:p")<CR>

" conceal
"let g:tex_conceal="adgm"

" format paragrath, see textwidth
vm Q gq
nma Q gqap

" clear trailing space
nn <leader>y :%s/\s\+$//<cr>

" insert date, press dd<tab> insert mode to generate current date
iab <expr> dd strftime("%c")

" windows
nn <C-w>\| <C-w>v
nn <C-w>" <C-w>s

" windows stuffs
se equalalways
"se eadirection

nn H 2zh
nn L 2zl
nn J <C-E>
nn K <C-Y>

" }}}

" Options {{{
if v:version >= 600
    se autoread
    se foldmethod=marker
    se printoptions=paper:letter
    se sidescrolloff=5
    se mouse=nvi
en

if has("multi_byte")
    if &termencoding == ""
        let &termencoding = &encoding
    en
    scriptencoding utf-8,latin1
    se encoding=utf-8
    setg fileencoding=utf-8
    se fileencodings=ucs-bom,utf-8,latin1
en

se nocompatible
se t_Co=256
se sw=4
se autowrite
se title

" use set nolist to disable those listchars by default
se nolist
se listchars=tab:\ \ ,extends:>,precedes:\ ,nbsp:~,trail:.

se signcolumn=no
se shm+=I "dont show version welcome page please

se ttyscroll=3
se ttyfast
se completepopup=height:10,width:80,highlight:Pmenu,border:on
se completeopt=menuone,menu,longest,noselect,popup,preview,fuzzy
se lazyredraw
se synmaxcol=174

" No swap files
se nobackup
se nowritebackup
se noswapfile

se hidden
se ignorecase
se smartcase " ignore case if search pattern is all lowercase, case-sensitive otherwise
se smarttab " insert tabs on the start of a line according to shiftwidth, not tabstop

se showmatch " set show matching parenthesis
se incsearch " show search matches as you type
se hlsearch " highlight search terms

se noai " always set autoindenting on
se nosi " no smart indenting
se copyindent " copy the previous indentation on autoindenting

se backspace=indent,eol,start " allow backspacing over everything in insert mode
se showmode " always show what mode we're currently editing in
se showcmd "
se scrolloff=4 " keep 4 lines off the edges of the screen when scrolling
se virtualedit=all " allow the cursor to go in to "invalid" places
se mouse=a " enable mouse

se pastetoggle=<F2> " when in insert mode, press <F2> to go to paste mode, where you can paste mass data that won't be autoindented
se fileformats="unix,dos,mac"
se wildmenu " make tab completion for files/buffers act like bash
se ruler " Show the cursor position all the time
se gdefault " :%s/foo/bar/g by default

se history=700 " remember more commands and search history
se undolevels=700 " use many muchos levels of undo

se softtabstop=4 shiftwidth=4 tabstop=4 " not tabs, but spaces

se expandtab
se shiftround " use multiple of shiftwidth when indenting with '<' and '>'

se rnu " turn on relative number
se nu "turn on number
se cursorline " highlight current light
"se cursorcolumn


" paragraph
se nowrap
se textwidth=80 " for coding
se wrapmargin=0
se formatoptions=vt " t: automatetic text wrapping

se makeprg=make

" fold
se foldenable
se foldmethod=manual
se foldlevelstart=0   " close all fold by default
" nnoremap <space> za  " space open/closes folds

se sidescroll=1
se sidescrolloff=10

se wildignore+=*.so,*.swp,*.zip,*.class,*.jar,*.gz,*pom.xml.org,*pyc,*.xls,*.svn
se wildignore+=*_build/*,*/coverage/*,*/target/*,*/tmp/*,*/lib/*,*/.settings/*,*/.git/*

" completion
se dict=/usr/share/dict/cracklib-small
se cpt=.,b,u,] " this specifies how key word complete works when using <c-p><c-n>
se wildmode=longest:full,full

" color
se background=dark
colo solarized

" transparent v-split windows
hi VertSplit ctermfg=10 ctermbg=NONE guibg=NONE
hi ErrorMsg term=NONE cterm=NONE  ctermbg=NONE ctermfg=9
hi Comment cterm=italic

" changing cursor in insert mode and normal mode
let &t_SI = "\e[6 q"
let &t_EI = "\e[4 q"

se wildoptions=fuzzy,pum

"}}}

" fillchars {{{
" open :UnicodeTable
" in insert mode <C-v> and type U00B7
se fcs=vert:│
se fcs+=fold:·
se fcs+=diff:\ "the leading space is used
"se fcs+=stl:┈
"se fcs+=stlnc:┈
" }}}

" Auto Groups {{{
aug configgroup
    au!
    au BufRead,BufNewFile *.html,*.xhtml,*.xml setl foldmethod=indent foldlevel=0 tabstop=2 shiftwidth=2
    au BufRead,BufNewFile *.vim setl shiftwidth=2 tabstop=2 foldmethod=marker foldlevel=0
    au BufNewFile,BufRead *.otl setl listchars=tab:\|\ ,extends:>,precedes:\ ,nbsp:~,trail:.
    au BufRead,BufNewFile *rc setl foldmethod=marker
    au BufRead,BufNewFile *.c,*.h,*.hh setl tabstop=4 softtabstop=4 expandtab foldmethod=manual foldlevel=0
    au BufRead,BufNewFile *.h,*.hpp,*.cc,*.cpp setl tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab foldmethod=manual foldlevel=0
    au BufRead,BufNewFile *.conf setl tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab foldmethod=marker foldlevel=0
aug END
"}}}

" Dispatch {{{
nn <leader>r :Dispatch<CR>
" }}}

" {{{ fugitive
nnoremap <leader>ga :Git add %:p<CR><CR>
nnoremap <leader>gs :Git<CR>
nnoremap <leader>gc :Git commit -v -q<CR>
nnoremap <leader>gt :Gcommit -v -q %:p<CR>
nnoremap <leader>gd :Gvdiffsplit<CR>
nnoremap <leader>ge :Gedit<CR>
nnoremap <leader>gr :Gread<CR>
nnoremap <leader>gw :Gwrite<CR><CR>
nnoremap <leader>gl :silent! Glog<CR>:bot copen<CR>
" nnoremap <leader>gp :Ggrep<Space>
" nnoremap <leader>gmv :Gmove<Space>
" nnoremap <leader>gbb :Git branch<Space>
nnoremap <leader>gbl :Git blame<cr>
" nnoremap <leader>go :Git checkout<Space>
nnoremap <leader>gps :Git push<CR>
nnoremap <leader>gpl :Git pull<CR>
" }}}

" Gui {{{
if has('gui_running')
    se guioptions-=m  "remove menu bar
    se guioptions-=T  "remove toolbar
    se guioptions-=r  "remove right-hand scroll bar
    se guioptions-=L  "remove left-hand scroll bar
    "set guifont=Cousine\ 10
    se guifont="Times New Roman 12"
    se nocursorcolumn

    se background=dark
    colo solarized
el
    " TODO: not sure why it doesnt work when I put it in Option section, somethings
    " need to load first!
    " Hack Disable tilder column "~" by change its color
    hi! EndOfBuffer ctermbg=bg ctermfg=bg guibg=bg guifg=bg
en
" }}}

" vim-emmet {{{
let g:user_emmet_leader_key=',e'
" }}}

" {{{ foldtext
fu! _foldtext()
    " clean up first line
    let line = substitute(getline(v:foldstart), '^\s*"\?\s*\|\s*"\?\s*{{' . '{\d*\s*', '', 'g') . ' '

    let foldchar = matchstr(&fillchars, 'fold:\zs.')

    " shorten first line, and added fold char ahead of first line
    "let foldtextstart = strpart('+' . repeat(foldchar, v:foldlevel*2) . line, 0, (winwidth(0)*2)/3)

    " simply use a portion of first line
    let foldtextstart = strpart(line, 0, (winwidth(0)*2)/3)

    let lines_count_text = printf("%10s", v:foldend - v:foldstart + 1 . ' lines') . ' '
    let foldtextend = lines_count_text . repeat(foldchar, 8)
    let foldtextlength = strlen(substitute(foldtextstart . foldtextend, '.', 'x', 'g')) + &foldcolumn
    retu foldtextstart . repeat(foldchar, winwidth(0)-foldtextlength) . foldtextend
endf
hi Folded cterm=italic ctermbg=NONE ctermfg=12
hi FoldColumn ctermbg=NONE
set foldtext=_foldtext()
" }}}

" Use man page inside vim {{{
ru! ftplugin/man.vim
" :se nolist | Man mbsync
" C-] to following link
" C-t to go back
" }}}

" Personalize highlighting {{{
hi SignColumn ctermfg=12 ctermbg=NONE
hi Search ctermbg=0 ctermfg=none
hi incsearch cterm=underline,bold ctermbg=136 term=underline,bold ctermfg=8

hi TabLine ctermfg=12 cterm=NONE ctermbg=0
hi TabLineFill ctermfg=12 cterm=NONE ctermbg=0
hi TabLineSel ctermfg=64 term=underline,reverse cterm=underline,reverse ctermbg=0

" CursorColumn, CursorLine, and CursorLineNr
hi CursorLineNr cterm=bold,italic ctermbg=NONE ctermfg=64
hi CursorLine  cterm=none ctermbg=0
hi CursorColumn ctermbg=NONE
hi LineNr ctermbg=NONE

" heading title looks better with yellow
hi Title term=NONE cterm=bold ctermfg=136
hi htmlH1 cterm=NONE cterm=NONE ctermfg=33
hi htmlH2 cterm=NONE cterm=NONE ctermfg=2
hi htmlH3 cterm=NONE cterm=NONE ctermfg=136

" spell error, warning, etc
hi Error cterm=italic ctermfg=166 ctermbg=0
hi SpellBad cterm=italic term=NONE
hi SpellCap cterm=none ctermfg=64

" function, identifier
hi Identifier ctermfg=14
hi Function ctermfg=251

" popup menu
hi Pmenu ctermfg=12 term=bold cterm=bold ctermbg=0
hi PmenuSel term=reverse cterm=reverse ctermfg=64 ctermbg=0
hi PmenuSbar ctermfg=12 term=bold cterm=bold ctermbg=0
" }}}

" vim-markdown {{{
" disable of conceal regardless of conceallevel
"let g:vim_markdown_conceal = 0
let g:markdown_folding = 1
let g:markdown_enable_folding = 1
" }}}

" Status line {{{
se ls=2

" status line
hi StatusLine ctermbg=0 cterm=none ctermfg=250
hi StatusLineNC ctermbg=NONE cterm=underline term=NONE ctermfg=10
hi User1 ctermbg=8 ctermfg=2 cterm=bold,underline
hi User2 ctermfg=250 ctermbg=8 cterm=underline
" }}}

" LSP {{{
" }}}

