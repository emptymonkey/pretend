# Pretend #
_pretend_ is a simple tool that lets you change your user identity ([uid](http://en.wikipedia.org/wiki/User_identifier_%28Unix%29)), group identity ([gid](http://en.wikipedia.org/wiki/Group_identifier)), and your list of supplementary group identities ([sgids](http://en.wikipedia.org/wiki/Group_identifier#Primary_vs._supplementary)) in Unix/Linux.  

**That's awesome! [1337 h4X0rZ rUL3!!](http://hackertyper.com/)**

Uh, no. This isn't an [exploit](http://en.wikipedia.org/wiki/Exploit_%28computer_security%29). This is a tool that lets you legitimately change your uid/gid/sgids using the [normal mechanisms](http://en.wikipedia.org/wiki/Syscall) of the underlying operating system. Changing identity is a privileged operation reserved for accounts with the [CAP_SETUID](http://lxr.linux.no/#linux+v3.9.4/include/uapi/linux/capability.h#L132) [capability](http://linux.die.net/man/7/capabilities), which normally means [root](http://en.wikipedia.org/wiki/Superuser#Unix_and_Unix-like). Actually, this tool is a *very* simple wrapper for the system calls that do exactly this:

<pre><code><a href="http://linux.die.net/man/2/setresuid">man setresuid</a>
<a href="http://linux.die.net/man/2/setresgid">man setresgid</a>
<a href="http://linux.die.net/man/2/setgroups">man setgroups</a>
</code></pre>
  

**So what then?! I'll just use _[su](http://linux.die.net/man/1/su)_!**

_su_ is good for switching uids for identities that already exist on the machine. However, if the identity isn't in /etc/passwd, then su will throw an error.

    empty@monkey:~$ sudo su foo
    Unknown id: foo
  

**But why would I ever need that?**

There are many reasons why you may want to quickly switch to a non-valid user id:

* Performing a forensic analysis of filesystems that originated from other hosts.
* Gaining access to a remote [root-squashed](http://en.wikipedia.org/wiki/Unix_security#Root_squash) filesystem as part of a [pentest](http://en.wikipedia.org/wiki/Penetration_test), for which you have no valid user map.
* Launching or accessing a [process](http://en.wikipedia.org/wiki/Process_%28computing%29), to meet a critical business need, after the process owner has left the company (and the account has been terminated).

## Usage ##

    Usage: pretend uid:gid[:gid1:gid2:...] COMMAND [ARGS]


## Installation ##

    git clone git@github.com:emptymonkey/pretend.git
    cd pretend
    make


## Examples ##

Basic use examples:

    empty@monkey:~$ id
    uid=1000(empty) gid=1000(empty) groups=1000(empty),4(adm),24(cdrom),25(floppy),29(audio)

    empty@monkey:~$ sudo pretend 5:26:103:110:50 id
    uid=5(games) gid=26(tape) groups=60(games),50(staff),103(ssh),110(kvm)

    empty@monkey:~$ sudo pretend 5:26:103:110:50 bash
    games@monkey:~$ whoami
    games

    empty@monkey:~$ sudo pretend 666:9000:9000 bash
    I have no name!@monkey:~$ whoami
    whoami: cannot find name for user ID 666
    I have no name!@monkey:~$ id
    uid=666 gid=9000 groups=9000

A more advanced usage example, looping over the entries of a remote root-squashed file system without read privileges:

    root@monkey:/mnt# ls -l target_vol
    drwxr-x---     2  8803     6211    4096 May 22  2011 foo/
    drwxr-x---     2  9836     6211    4096 Jun  7  2012 bar/
    drwxr-x---     3  7438     6211    4096 Apr 19  2007 baz/

    root@monkey:/mnt# for i in `ls --color=no -1`; do pretend `ls -ld $i | awk '{print $3,$4}' OFS=:` ls -al; done
