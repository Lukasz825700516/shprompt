# shprompt
Simple c prompt with git repository and branch information

## install
1. Clone repository
1. make
1. Put into your .bashrc this lines:
```sh
function prompt_command {
	export PS1=$( path/to/shprompt )
}
export PROMPT_COMMAND="prompt_command"

```

## todo
- make install
