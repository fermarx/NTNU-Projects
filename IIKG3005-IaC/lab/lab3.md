# Lab 3 - Puppet the language 

## Lab tutorials

1.  Launch the Puppet learning-VM `Puppet Learning VM 2019.0.2 6.10` in
    SkyHiGh (you will find it in Glance, choose a flavor with 2 vcpus)
    and do everything you are asked to do in the chapters
    
      - Welcome
    
      - Hello Bolt
    
      - Hello Puppet
    
      - Agent Run
    
      - Manifests and Classes
    
      - Package File Service
    
      - Variables and Templates
    
      - Class Parameters
    
      - Facts
    
      - Conditional Statements
    
      - The Forge
    
    of the Quest Guide.
    
    To access the Quest Guide and get started, enter the learning-VM’s
    IP address in your browser’s address bar (be sure to use
    `http://<IPADDRESS>` for the Quest Guide, as `https://<IPADDRESS>`
    will take you to the Puppet Enterprise console) or see
    [Summary](https://github.com/puppetlabs/puppet-quest-guide/blob/master/summary.md)
    
    You can log into the learning-VM with ssh as root, but you need to
    access the console in Horizon to see the password on the splash page
    (make a note of this password).
    
    If you get the error "something went wrong, connection closed" when trying to access the console through Horizon, try refreshing the webpage. If that does not work, delete the instance and create a new one. Sometimes accessing the console right after creating the instance works. 

    If you want you can insert your public key to the Puppet Learning VM so that you can login without the password. First get your public key that we created in lab 1 (run this command on the computer you have the openstack client installed on)
        
        openstack keypair show --public-key KEY_NAME
        
    Log in to the Puppet Learning VM using a password, and do the following (note that you need to insert your PUBLIC_KEY)

        mkdir -m 700 /root/.ssh
        echo "PUBLIC_KEY" > /root/.ssh/authorized_keys
        chmod 600 /root/.ssh/authorized_keys
        restorecon -r /root/.ssh

## Review questions and problems
To install a server on SkyHiGh that you can use to work with Puppet, see the [following instructions.](https://gitlab.stud.idi.ntnu.no/jhklemet/iikg3005-labs/-/blob/4a99b7489817ba6c41cdacba8e221988653e4617/puppet_vm.md) 

Hint. Use the [puppet resource type documentation.](https://puppet.com/docs/puppet/7/resource_types.html)

1.  What is a *resource type* in Puppet? give three examples of resource
    types.

2.  What is the Puppet Resource Abstraction Layer (RAL)?

3.  You can get the ip (v4) address of a host with `facter ipaddress`,
    but how do you access this same information as a top-scope variable
    in a Puppet manifest?

4.  Given that all services your team is responsible for are
    containerized (run in containers), how does this simplify server
    management?

5.  Write a Puppet manifest that ensures your home directory
    (`/home/ubuntu`) is only accessible by you (permission `0750`).

6.  Write a Puppet manifest that ensures `/home/ubuntu/public_html` is a
    symbolic link to `/var/www/ubuntu`. Is `/var/www/ubuntu` created if
    it does not exist?

7.  Write a Puppet manifest that ensures that the packages `vim`,
    `nano`, `jed` and `jove` are installed. Use an array to store the
    list of packages so you only need to declare the `package` resource
    type once.

8.  Write a Puppet manifest that ensures the user `data` exists with a
    encrypted password `$6$38Tdw...` and belongs to the `sudo` group.
    Also make sure the group `data` exists before ensuring the user
    `data` exists.

9.  Write a Puppet manifest which does the following *in sequence*:
    
    1.  ensure the ssh package is in its latest version.
    
    2.  edit the config file `sshd_config`, use the following resource
        declaration for this:
        
            augeas { 'sshd_config':
              context => '/files/etc/ssh/sshd_config',
              changes => 'set X11Forwarding yes',
            }
    
    3.  make sure that the ssh service is running and restarted if its
        configuration has been changed.

10. Write a Puppet manifest that makes sure the software package
    `mybackup` is installed in its latest version, and creates a cron
    entry to run the command  
    `mybackup /home/data` as root every half hour. The manifest must
    include a notify relationship between the package and cron resource
    declarations.

    *Note. You will not be able to execute this task as there is no real package called “mybackup”).*

