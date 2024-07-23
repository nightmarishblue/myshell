# myshell
## the shell i built in a week and had to document for a month 🙂

Commandline UNIX shell that boasts:
- Standard I/O redirection
- Background execution of jobs
- Configurability via ~/.mshrc
- (limited) environment variable expansion
- Shell aliases!

Run `make build` and check `bin/` for the binary.

If you are looking for msh's manual, try looking in `manual/`.

## Builtins (the important ones)
Command | Effect
:---: | ---
`help` | Open your pager with `msh`'s manual.
`quit` | Close msh.
`cd [directory]` | Change working directory to `directory` if it exists. If no args are given, print the current working directory.
`echo [words...]` | Print the given `words` in order, separated by a space each.
`environ` | Print all environment variables in the form `VAR=VAL`.
`putenv <name> <value>` | Set `name` to `value` in `msh`'s environment. Child processes will inherit the variable and its value.
`clrenv <name>` | Unset `name` in msh's environment. Child processes will no longer receive this variable upon creation.
`alias [<shortcut> <command>]` | Makes `shortcut` an alias for `command`. If no args are given, print all aliases in the form `ALIAS=CMD`.
`dealias [shortcut]` | Clears `shortcut`. If none is given, clear all aliases.
`source <filename>` | Sources `filename`. This executes every line in it, with changes applying to this session.
