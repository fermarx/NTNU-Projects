# Set up a server for Puppet exercises


1. Create a stack containing Ubuntu server (replace MY_KEY with the name of your keypair)

        openstack stack create -t https://folk.ntnu.no/erikhje/single_linux_instance.yaml --parameter key_name=MY_KEY puppetstack  

2. Login to the server using SSH
        ssh -i MY_KEY.pem ubuntu@FLOATING_SERVER_IP_ADDRESS

3. Install the most recent version of Puppet on the server (NOTE:
    PUPPET COMMANDS SHOULD BE RUN AS ROOT (`sudo -s` or `sudo -i`) SINCE
    PUPPET IS DOING SYSTEM MAINTENANCE):
    
        # 
        # https://docs.puppet.com/puppet/latest/reference/install_linux.html
        #
        
        wget https://apt.puppetlabs.com/puppet6-release-bionic.deb
        sudo dpkg -i puppet6-release-bionic.deb
        sudo apt-get update
        sudo apt-get install puppet-agent
        # add Puppet binaries to PATH:
        echo 'export PATH=$PATH:/opt/puppetlabs/bin/' >> ~/.bashrc
        # source .bashrc to apply:
        . ~/.bashrc
