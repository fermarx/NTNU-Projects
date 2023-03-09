# Lab 2 - Infrastructure Definition Tools (OpenStack Heat)

## Lab tutorial

1.  Create a new network with two Ubuntu instances by using the Heat
    template `servers_in_new_neutron_net.yaml` from (remember to always
    examine code that you reuse, but this is from the repo of the
    OpenStack Heat developers so this is probably good code, and reusing
    good code is a best practice we should enforce)
    [openstack/heat-templates](https://github.com/openstack/heat-templates/blob/master/hot)
    
        openstack stack create -t servers_in_new_neutron_net.yaml \
         -e heat_demo_env.yaml heat_demo
    
    Your environment file `heat_demo_env.yaml` should look something
    like this (the environment file is for enforcing what should be a
    well known principle for you: *separating code and data*)
    
        parameters:
          key_name: KEY_NAME
          image: Ubuntu Server 18.04 LTS (Bionic Beaver) amd64
          flavor: m1.small
          public_net: ntnu-internal
          private_net_name: net1
          private_net_cidr: 192.168.1XY.0/24
          private_net_gateway: 192.168.1XY.1
          private_net_pool_start: 192.168.1XY.200
          private_net_pool_end: 192.168.1XY.250
    
    If you get an error immediately when trying to create a stack you
    probably have a syntax error (e.g. your Yaml file does not have
    correct indentation or similar). If syntax is OK, but the stack
    fails to create e.g. `CREATE_FAILED` message or similar, try
    something like  
    `openstack stack event list heat_demo --nested-depth 3`

## Review questions and problems

1.  How does a Heat resource retrieve/make use of a value passed as a
    parameter to the template? How does a Heat resource reference
    another Heat resource?

2.  Describe the main requirements for an Infrastructure Definition
    Tool.

3.  What is a “stack”? What properties/characteristics has a stack?

4.  What is a good rule-of-thumb (guiding principle) when splitting up
    your infrastructure into multiple stacks?

5.  After you have completed this chapters lab tutorial, modify the
    template in such a way that the two servers can be different (e.g.
    an Ubuntu and a Windows instance). Also allow for a list of security
    groups to be added as parameters to each server. Delete the Heat
    stack from the previous exercise and launch it again but this time
    with an Ubuntu and a Windows instance (with the linux and windows
    security groups, respectively).

## Challenges (extra)

Challenge 1.

How can you provision for the following infrastructure using modular Openstack Heat code (kind of service stack pattern)?
- A webservice with 3 redundant VMs
- A database cluster consisting of 3 VMs
- A loadbalancer VM 

You do not need to install the software, just provision the compute, networking and storage resources 
required. Hint. https://docs.openstack.org/heat/pike/template_guide/composition.html


Challenge 2.
How can we use openstack Heat to install software on the VMs that we provision? Try installing, for example, a loadbalancer (nginx). 

Send your answers here:
https://forms.gle/8YKL547r8E9mdynb7
