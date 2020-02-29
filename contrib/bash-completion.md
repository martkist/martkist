Bash Completion
---------------------

The following script provide bash completion functionality for `sycoind` and `martkist-cli`.

* `contrib/martkistd.bash-completion`
* `contrib/martkist-cli.bash-completion`
* `contrib/_osx.bash-completion`

### OSX ###
Use `brew` to install `bash-completion` then update `~/.bashrc` to include the completion scripts and helper functions to provide `have()` and `_have()` on OSX.

The example assumes Martkist was compiled in `~/martkist` and the scripts will be availabe in `~/martkist/contrib`, however they can be moved to a different location.

```
brew install bash bash-completion
sudo bash -c 'echo /usr/local/bin/bash >> /etc/shells'
chsh -s /usr/local/bin/bash

BASHRC=$(cat <<EOF
if [ -f $(brew --prefix)/etc/bash_completion ]; then
  . $(brew --prefix)/etc/bash_completion
fi
. ~/martkist/contrib/_osx.bash-completion
. ~/martkist/contrib/martkistd.bash-completion
. ~/martkist/contrib/martkist-cli.bash-completion
EOF
)

grep -q "/etc/bash_completion" ~/.bashrc || echo "$BASHRC" >> ~/.bashrc
. ~/.bashrc

```