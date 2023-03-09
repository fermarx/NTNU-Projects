# Lab 1 - Introduction to OpenStack
## Lab tutorial

1.  You will be using NTNUs private OpenStack cloud
    [SkyHiGh](https://www.ntnu.no/wiki/display/skyhigh). OpenStack is an
    cloud solution composed of many sub projects. SkyHiGh is accessible
    as long as you are in the NTNU network (use VPN if you need access
    from outside).
    
    Each OpenStack project has its own client, but these individual
    clients are now deprecated in favor of a common `openstack` client
    to standardize and simplify the interface :
    
    > The following individual clients are deprecated in favor of a
    > common client. Instead of installing and learning all these
    > clients, we recommend installing and using the OpenStack client.
    
    You are expected to solve this exercise using the [Openstack
    Command-line
    Client](https://docs.openstack.org/python-openstackclient/latest/)
    
    (note: in some special cases you might have to still use to old
    commands if the `openstack` command does not support the operation)
    
    You will be provided with all commands you need to execute most of
    the time, but make sure you understand what is happening (not just
    copy and paste without thinking). This is the foundation for scaling
    up operations later in the course.
    
    *Parts of commands that are written in UPPERCASE are meant to be
    replaced by you*.
    
    Tip: View your infrastructure in  
    <https://skyhigh.iik.ntnu.no/horizon/project/network_topology/>  
    as we go along.
    
      - To access to lab from outside networks, you need to be connected
        by VPN to the NTNU network, see [Install
        VPN](https://innsida.ntnu.no/wiki/-/wiki/English/Install+VPN)
    
      - For RDP on Windows, use built-in Remote Desktop Connection
        (`mstsc`), if you are using Windows 8 or newer, consider
        applying this fix (you probably have to apply only the first
        one) first:  
        <http://chall32.blogspot.no/2012/04/fixing-remote-desktop-annoyances.html>
        
        On Mac, get Microsoft Remote Desktop connection from the App
        store.
        
        On Linux, use something like:  
        `xfreerdp /u:Admin +clipboard /w:1366 /h:768 /v:SERVER_IP`
    
    <!-- end list -->
    
    1.  Decide on where you want your work environment to be: on your
        laptop or on a server (e.g. `login.stud.ntnu.no`). If you use
        your laptop, you need to install the
        [python-openstackclient](https://github.com/openstack/python-openstackclient).
        If you ssh to a server you are strongly advised to use tmux (or
        screen if you prefer) session. If you just want to get started
        fast, I recommend you ssh to `login.stud.ntnu.no`.
        
        If you use Windows, you can install OpenSSH with [Installation
        of OpenSSH For Windows Server 2019 and Windows
        10](https://docs.microsoft.com/en-us/windows-server/administration/openssh/openssh_install_firstuse).
    
    2.  Familiarize yourself with the most widely used OpenStack CLI
        commands:
        
            openstack -h | less
        
         Set environment variables using the OpenStack RC file to login as a "service" user 
        (so you don't have to store your NTNU password in environment variables):
        
        1. Login to SkyHiGh GUI using NTNU user -> "Object Store" -> "Containers" -> download the file with the service user password for your project

        2.  Login to SkyHiGh GUI using service user (choose "Openstack accounts") -> “API Access” -> “Download OpenStack RC
            file” 
        
        3.  Copy this file to your work environment (e.g.  
            `scp PRIV_user-openrc.sh user@login.stud.ntnu.no:`)
        
        4. Insert the service user password into the RC-file by changing the following lines:
        ```
        # With Keystone you pass the keystone password.
        echo "Please enter your OpenStack Password for project $OS_PROJECT_NAME as user $OS_USERNAME: "
        read -sr OS_PASSWORD_INPUT
        export OS_PASSWORD=$OS_PASSWORD_INPUT
        ```
        To this (remember to put in the password of the service user): 
        ```
        # With Keystone you pass the keystone password.
        # echo "Please enter your OpenStack Password for project $OS_PROJECT_NAME as user $OS_USERNAME: "
        # read -sr OS_PASSWORD_INPUT
        export OS_PASSWORD="service user password"
        ```

        5.  Source the file: `. PRIV_user-openrc.sh`
        
        6.  Check that you now can run commands without problems, e.g.  
            `openstack keypair list`
    
    3.  Create a key pair (e.g. use your username as key name) and change the persmissions of the file containing the private key (otherwise ssh will complain). Take good care of the private key\!
        
            openstack keypair create KEY_NAME > KEY_NAME.pem
            chmod 400 KEY_NAME.pem
    
    4.  Create a network.
        
            openstack network create net1
    
    5.  Create a subnet `192.168.1XY.0/24` where `XY` is your choice.
        
            openstack subnet create subnet1 --network net1 \
             --subnet-range 192.168.1XY.0/24
    
    6.  Create a router.
        
            openstack router create router1
    
    7.  Set the router’s gateway to be in `ntnu-internal`.
        
            openstack router set router1 --external-gateway ntnu-internal
    
    8.  Add an interface on the router to your subnet.
        
            openstack router add subnet router1 subnet1
        
        You now have a network where you can boot your servers. If you
        need to delete this setup, deleting it can sometimes be tricky
        due to dependencies, but something like this should work (it
        also worth noting that sometimes it is easier to delete stuff
        using the GUI (Horizon) than the command-line):
        
            # deleting what you just created:
            openstack router remove subnet router1 subnet1
            openstack router delete router1
            openstack subnet delete subnet1
            openstack network delete net1
    
    9.  Boot the most recent LTS Ubuntu release available in glance (the
        image store, `openstack image list`) in your network. (Remember
        to use your key, it will be injected into the instance by
        cloud-init, and you will use it to log in later).
        
            openstack server create --image NAME_OR_ID --flavor NAME_OR_ID \
             --nic net-id=UUID --key-name KEY_NAME INSTANCE_NAME
    
        You can list available flavors by using the command `openstack flavor list --sort-column VCPUs`.

    10. Boot the most recent Windows <ins>server</ins> available in glance in your
        network. Remember to use your key, it will be used by
        cloudbase-init to injected a random password for the user
        `Admin` which you can retrieve with:
        
            nova get-password INSTANCE_NAME KEY_NAME.pem
    
    11. Create floating ips in the `ntnu-internal` and associate them
        with the Ubuntu and Windows instance.
        
            openstack floating ip create ntnu-internal
            openstack server add floating ip INSTANCE_NAME_OR_ID FLOATING_IP_ADDRESS
    
    12. Add a security rule to the `default` security group to allow
        ping.
        
            openstack security group rule create --protocol icmp \
             --remote-ip 0.0.0.0/0 default
            # to list all the rules in this security group:
            openstack security group rule list default
    
    13. Add security rules for http and https also to the `default`
        security group.
        
            openstack security group rule create --protocol tcp \
             --remote-ip 0.0.0.0/0 --dst-port 80 default
            openstack security group rule create --protocol tcp \
             --remote-ip 0.0.0.0/0 --dst-port 443 default
    
    14. Create a security group `linux` with a rule allowing ssh. Add
        this security group to the Ubuntu instance.
        
            openstack security group create linux
            openstack security group rule create --protocol tcp \
             --remote-ip 0.0.0.0/0 --dst-port 22 linux
            openstack server add security group INSTANCE_NAME linux
    
    15. Create a security group `windows` with a rule allowing rdp. Add
        this security group to the Windows instance.
        
            openstack security group create windows
            openstack security group rule create --protocol tcp \
             --remote-ip 0.0.0.0/0 --dst-port 3389 windows
            openstack server add security group INSTANCE_NAME windows
    
    16. Create two 1GB volumes with display names `linux_vol` and
        `windows_vol` and attach them to the Ubuntu and Windows
        instances, respectively.
        
            openstack volume create --size 1 linux_vol
            openstack volume create --size 1 windows_vol
            openstack server add volume INSTANCE_NAME_OR_ID VOLUME_NAME_OR_ID
    
    17. Log in to the Ubuntu instance, format, create filesystem and
        mount the new volume on `/myvol`. You need to run the commands on the Ubuntu instance as root (`sudo su`).

            ssh -i KEY_NAME.pem ubuntu@FLOATING_IP_ADDRESS

            cfdisk /dev/vdb
            mkfs.ext4 /dev/vdb
            mkdir /myvol
            mount /dev/vdb /myvol
    
    18. Log in to the Windows instance (username `Admin`), use `Disk
        management` to bring online, initialize, create volume and mount
        the new volume on `D:\`.
        
            # Right click start button, Disk management, 
            # bring online, initialize, Create volume
    
    19. Disable SMBv1 on the Windows instance.
        
            Get-WindowsFeature FS-SMB1
            Disable-WindowsOptionalFeature -Online -FeatureName smb1protocol
            #
            # some other windows stuff you might want to do:
            ##############################################################
            # on Windows 10 and newer, to get the ssh command you can do:
            # PowerShell -> Run as administrator
            powershell Set-ExecutionPolicy Bypass -Scope Process -Force; \ 
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \ 
            iex((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
            # Restart PowerShell -> Run as administrator
            # Visit https://chocolatey.org/packages and check that most 
            # recent version of nano and openssh are marked as 
            # approved/trusted
            choco install nano openssh
            ##############################################################
    
    20. Install nginx on the Ubuntu instance, verify that it is up and
        running by browsing its floating ip.
    
    21. Delete everything you have created except your keypair and your
        security groups.

## Review questions and problems

1.  What is a *security group* in OpenStack? What do you use it for?

2.  What is *automation fear*? How can it happen? (Tip: check the book, ch.2, "Principle: Minimize Variation")

3.  In pseudo code, write an ordered list (reflecting the sequence you
    would run) of `openstack` (including all command line arguments) to set up your own router,
    network and Ubuntu server in SkyHiGh in such a way that you can
    reach it with ssh from other hosts in the NTNU-network (similar to
    what we have done in the lab).
