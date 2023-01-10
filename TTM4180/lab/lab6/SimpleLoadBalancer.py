from pox.core import core
from pox.openflow import *
import pox.openflow.libopenflow_01 as of
from pox.lib.packet.arp import arp
from pox.lib.packet.ipv4 import ipv4
from pox.lib.addresses import EthAddr, IPAddr
log = core.getLogger()
import time
import random
import pox.log.color


IDLE_TIMEOUT = 10
LOADBALANCER_MAC = EthAddr("00:00:00:00:00:FE") # <requested mac address>
ETHERNET_BROADCAST_ADDRESS=EthAddr("ff:ff:ff:ff:ff:ff")
N_SERVERS = 3

class SimpleLoadBalancer(object):

	def __init__(self, service_ip, server_ips = []):
		core.openflow.addListeners(self)
		self.SERVERS = {} # IPAddr[SERVER_IP]={'server_mac':EthAddr(SERVER_MAC),'port': PORT_TO_SERVER}
		self.CLIENTS = {} # IPAddr[CLIENT_IP]={'client_mac':EthAddr(CLIENT_MAC),'port': PORT_TO_CLIENT}
		self.LOADBALANCER_MAP = {} # Mapping between clients and servers
		self.LOADBALANCER_IP = service_ip
		self.SERVER_IPS = server_ips
		self.ROBIN_COUNT = 0


	def _handle_ConnectionUp(self, event):
		self.connection = event.connection
		log.debug("FUNCTION: _handle_ConnectionUp")
		# Send ARP Requests to learn the MAC address of all Backend Servers.
		for ip in self.SERVER_IPS:
			self.send_arp_request(self.connection, ip)
		log.debug("Sent ARP Requests to all servers")


	def round_robin(self):
		log.debug("FUNCTION: round_robin")
		# Implement logic to choose the next server according to the Round Robin scheduling algorithm
		# 4 Clients and 3 servers
		server_names = self.SERVER_IPS
		if self.ROBIN_COUNT < len(self.SERVER_IPS):
			server = server_names[self.ROBIN_COUNT]
			self.ROBIN_COUNT += 1			
		else:
			self.ROBIN_COUNT = 0
			server = server_names[self.ROBIN_COUNT]
			self.ROBIN_COUNT += 1	
		log.info("Round robin selected: %s" % server)
		return server


	def update_lb_mapping(self, client_ip):
		log.debug("FUNCTION: update_lb_mapping")
		if client_ip in self.CLIENTS.keys():
			if client_ip not in self.LOADBALANCER_MAP.keys():
				selected_server = self.round_robin() # select the server which will handle the request
				self.LOADBALANCER_MAP[client_ip]=selected_server


	def send_arp_reply(self, packet, connection, outport):
		log.debug("FUNCTION: send_arp_reply")

		arp_rep = arp() # Create an ARP reply; Help from https://noxrepo.github.io/pox-doc/html/#example-arp-messages
		arp_rep.hwtype = arp_rep.HW_TYPE_ETHERNET
		arp_rep.prototype = arp_rep.PROTO_TYPE_IP
		arp_rep.hwlen = 6
		arp_rep.protolen = arp_rep.protolen
		arp_rep.opcode = arp.REPLY # Set the ARP TYPE to REPLY

		arp_rep.hwdst = packet.src # Set MAC destination
		arp_rep.hwsrc = LOADBALANCER_MAC # Set MAC source <requested mac address>

		# Reverse the src, dest to have an answer; Help from l3_learning.py
		arp_rep.protosrc =  packet.payload.protodst # Set IP source <IP of requested mac-associated machine>
		arp_rep.protodst = packet.payload.protosrc # Set IP destination
		
		eth = ethernet() # Create an ethernet frame and set the arp_rep as it's payload.
		# eth = ethernet(type=packet.type, src=LOADBALANCER_MAC, dst=packet.src)

		eth.type = packet.ARP_TYPE # Set packet Typee
		eth.dst = packet.src # Set destination of the Ethernet Frame ######
		eth.src = LOADBALANCER_MAC # Set source of the Ethernet Frame <requested mac address>
		eth.set_payload(arp_rep)
		
		msg = of.ofp_packet_out() # create the necessary Openflow Message to make the switch send the ARP Reply
		msg.data = eth.pack()
		
		msg.actions.append(of.ofp_action_output(port = of.OFPP_IN_PORT)) # Append the output port which the packet should be forwarded to; Help from Lab5 T4
		msg.in_port = outport
		connection.send(msg)


	def send_arp_request(self, connection, ip):
		# Help from https://en.wikipedia.org/wiki/Address_Resolution_Protocol#Example

		log.debug("FUNCTION: send_arp_request")

		arp_req = arp() # Create an instance of an ARP REQUEST PACKET
		arp_req.hwtype = arp_req.HW_TYPE_ETHERNET
		arp_req.prototype = arp_req.PROTO_TYPE_IP
		arp_req.hwlen = 6
		arp_req.protolen = arp_req.protolen
		arp_req.opcode = arp_req.REQUEST # Set the opcode
		arp_req.protodst = ip # IP the load balancer is looking for; From the example: 192.168.0.55
		arp_req.hwsrc = LOADBALANCER_MAC # Set the MAC source of the ARP REQUEST 
		arp_req.hwdst = ETHERNET_BROADCAST_ADDRESS # Set the MAC address in such a way that the packet is marked as a Broadcast; FF:FF:FF:FF:FF:FF
		arp_req.protosrc = self.LOADBALANCER_IP # Set the IP source of the ARP REQUEST

		eth = ethernet() # Create an ethernet frame and set the arp_req as it's payload.
		eth.type = eth.ARP_TYPE # Set packet Type
		eth.dst = ETHERNET_BROADCAST_ADDRESS # Set the MAC address in such a way that the packet is marked as a Broadcast; FF:FF:FF:FF:FF:FF
		eth.set_payload(arp_req)

		msg = of.ofp_packet_out() # create the necessary Openflow Message to make the switch send the ARP Request
		msg.data = eth.pack()
		msg.actions.append(of.ofp_action_nw_addr(of.OFPAT_SET_NW_DST,ip))

		msg.actions.append(of.ofp_action_output(port = of.OFPP_FLOOD)) # append an action to the message which makes the switch flood the packet out; Help from Lab5 T4
		msg.in_port = of.OFPP_NONE
		connection.send(msg)


	def install_flow_rule_client_to_server(self,event, connection, outport, client_ip, server_ip):
		log.debug("FUNCTION: install_flow_rule_client_to_server")
		self.install_flow_rule_server_to_client(connection, event.port, server_ip,client_ip)

		msg = of.ofp_flow_mod() # Create an instance of the type of Openflow packet you need to install flow table entries
		msg.idle_timeout = IDLE_TIMEOUT

		msg.match.dl_type = ethernet.IP_TYPE

 		# MATCH on destination and source IP; Help from https://noxrepo.github.io/pox-doc/html/#match-structure
		msg.match.nw_dst = self.LOADBALANCER_IP
		msg.match.nw_src = client_ip

		# SET dl_addr source and destination addresses; Help from https://noxrepo.github.io/pox-doc/html/#openflow-actions
		msg.actions.append(of.ofp_action_dl_addr.set_src(LOADBALANCER_MAC))
		msg.actions.append(of.ofp_action_dl_addr.set_dst(self.SERVERS[server_ip].get("server_mac")))
		
		# SET nw_addr source and destination addresses
		msg.actions.append(of.ofp_action_nw_addr.set_src(client_ip))
		msg.actions.append(of.ofp_action_nw_addr.set_dst(server_ip))

		msg.actions.append(of.ofp_action_output(port=outport)) # Set Port to send matching packets out; Help from Lab5 T4
		self.connection.send(msg)
		log.info("Installed flow rule: %s -> %s" % (client_ip,server_ip))
		

	def install_flow_rule_server_to_client(self, connection, outport, server_ip, client_ip):
		log.debug("FUNCTION: install_flow_rule_server_to_client")

		msg = of.ofp_flow_mod() # Create an instance of the type of Openflow packet you need to install flow table entries; Help from https://noxrepo.github.io/pox-doc/html/#match-structure
		msg.idle_timeout = IDLE_TIMEOUT

		msg.match.dl_type=ethernet.IP_TYPE

		# MATCH on destination and source IP
		msg.match.nw_src = server_ip
		msg.match.nw_dst = client_ip

		# SET dl_addr source and destination addresses; Help from https://noxrepo.github.io/pox-doc/html/#openflow-actions
		msg.actions.append(of.ofp_action_dl_addr.set_dst(self.CLIENTS[client_ip].get("client_mac")))
		msg.actions.append(of.ofp_action_dl_addr.set_src(LOADBALANCER_MAC))

		# SET nw_addr source and destination addresses
		msg.actions.append(of.ofp_action_nw_addr.set_src(self.LOADBALANCER_IP))
		msg.actions.append(of.ofp_action_nw_addr.set_dst(client_ip))

		msg.actions.append(of.ofp_action_output(port=outport)) # Set Port to send matching packets out; Help from Lab5 T4
		self.connection.send(msg)
		log.info("Installed flow rule: %s -> %s" % (server_ip,client_ip))


	def _handle_PacketIn(self, event):
		log.debug("FUNCTION: _handle_PacketIn")
		packet = event.parsed
		connection = event.connection
		inport = event.port
		if packet.type == packet.LLDP_TYPE or packet.type == packet.IPV6_TYPE:
			log.info("Received LLDP or IPv6 Packet...")

		elif packet.type == packet.ARP_TYPE: # Handle ARP Packets; Help from https://noxrepo.github.io/pox-doc/html/#example-arp-messages
			log.debug("Received ARP Packet")
			response = packet.payload
			if packet.payload.opcode == packet.payload.REPLY: # Handle ARP replies
				log.debug("ARP REPLY Received")
				if response.protosrc not in self.SERVERS.keys():
					# Add Servers MAC and port to SERVERS dict if there is no ip in the server dict
					# self.send_arp_request(packet, connection, packet.next.dstip)
					self.SERVERS[IPAddr(response.protosrc)]={'server_mac':EthAddr(packet.payload.hwsrc),'port': inport}

			elif packet.payload.opcode == packet.payload.REQUEST: # Handle ARP requests
				log.debug("ARP REQUEST Received")
				if (response.protosrc not in self.SERVERS.keys() and response.protosrc not in self.CLIENTS.keys()): # if the IP is not in the clienet disct, it adds it
					self.CLIENTS[response.protosrc]={'client_mac':EthAddr(packet.payload.hwsrc),'port':inport} #insert client's ip  mac and port to a forwarding table
									
				if (response.protosrc in self.CLIENTS.keys() and response.protodst == self.LOADBALANCER_IP):
					log.info("Client %s sent ARP req to LB %s"%(response.protosrc,response.protodst))
					# Load Balancer intercepts ARP Client -> Server
					self.send_arp_reply(packet, connection, inport) # Send ARP Reply to the client, include the event.connection object

				elif (response.protosrc in self.SERVERS.keys() and response.protodst in self.CLIENTS.keys()):
					log.info("Server %s sent ARP req to client"%response.protosrc)
					# Load Balancer intercepts ARP from Client <- Server
					self.send_arp_reply(packet, connection, inport) # Send ARP Reply to the Server, include the event.connection object
					
				
				else:
					log.info("Invalid ARP request")
				######################################### MIRAR

		elif packet.type == packet.IP_TYPE: # Handle IP Packets
			log.debug("Received IP Packet from %s" % packet.next.srcip)
			# Handle Requests from Clients to Servers
			# Install flow rule Client -> Server
			if (packet.next.dstip == self.LOADBALANCER_IP and packet.next.srcip not in self.SERVERS.keys()): # Check if the packet is destined for the LB and the source is not a server

				self.update_lb_mapping(packet.next.srcip)
				client_ip =  packet.payload.srcip # Get client IP from the packet
				server_ip = self.LOADBALANCER_MAP.get(packet.next.srcip)
				outport = self.SERVERS[server_ip].get('port') # Get Port of Server
				self.install_flow_rule_client_to_server(event,connection, outport, client_ip,server_ip)
				
				# Either use the code below to create a new Ethernet packet, or use Buffer_Id
				# Used the code with help of https://noxrepo.github.io/pox-doc/html/#ethernet-ethernet
				eth = ethernet()
				eth.type = ethernet.IP_TYPE # Set the correct Ethernet TYPE, to send an IP Packet
				eth.dst = self.SERVERS[server_ip].get('server_mac') # Set the MAC destination
				eth.src = LOADBALANCER_MAC # Set the MAC source
				eth.set_payload(packet.next)

				# Send the first packet (which was sent to the controller from the switch)
				# to the chosen server, so there is no packetloss
				msg = of.ofp_packet_out() # Create an instance of a message which can be used to instruct the switch to send a packet
				msg.data = eth.pack()
				msg.in_port = inport # Set the correct in_port
				
				msg.actions.append(of.ofp_action_dl_addr.set_src(LOADBALANCER_MAC)) # Add an action which sets the MAC source to the LB's MAC
				
				msg.actions.append(of.ofp_action_dl_addr.set_dst(self.SERVERS[server_ip].get('server_mac'))) # Add an action which sets the MAC destination to the intended destination...

				msg.actions.append(of.ofp_action_nw_addr.set_src(client_ip)) # Add an action which sets the IP source
				msg.actions.append(of.ofp_action_nw_addr.set_dst(server_ip)) # Add an action which sets the IP destination
				msg.actions.append(of.ofp_action_output(port=outport)) # Add an action which sets the Outport

				connection.send(msg)

			# Handle traffic from Server to Client
			# Install flow rule Client <- Server
			elif packet.next.dstip in self.CLIENTS.keys(): #server to client
				log.info("Installing flow rule from Server -> Client")
				if packet.next.srcip in self.SERVERS.keys():

					server_ip =  packet.payload.srcip # Get the source IP from the IP Packet
					client_ip = self.LOADBALANCER_MAP.keys()[list(self.LOADBALANCER_MAP.values()).index(packet.next.srcip)]
				######################################
					outport=int(self.CLIENTS[client_ip].get('port'))
					self.install_flow_rule_server_to_client(connection, outport, server_ip,client_ip)

					# Either use the code below to create a new Ethernet packet, or use Buffer_Id 
					# As did before, I have used the code below 
					eth = ethernet()
					eth.type = ethernet.IP_TYPE # Set the correct Ethernet TYPE, to send an IP Packet
					eth.dst = self.CLIENTS[client_ip].get('client_mac') # Set the MAC destination
					eth.src = LOADBALANCER_MAC # Set the MAC source
					eth.set_payload(packet.next)

					# Send the first packet (which was sent to the controller from the switch)
					# to the chosen server, so there is no packetloss
					msg =  of.ofp_packet_out() # Create an instance of a message which can be used to instruct the switch to send a packet
					msg.data = eth.pack()
					msg.in_port = inport # Set the correct in_port

					msg.action.append(of.ofp_action_dl_addr.set_src(self.LOADBALANCER_IP)) # Add an action which sets the MAC source to the LB's MAC
					msg.action.append(of.ofp_action_dl_addr.set_dst(self.CLIENTS[client_ip].get('client_mac'))) # Add an action which sets the MAC destination to the intended destination...

					msg.action.append(of.ofp_action_nw_addr.set_src(self.LOADBALANCER_IP)) # Add an action which sets the IP source
					msg.action.append(of.ofp_action_nw_addr.set_dst(client_ip)) # Add an action which sets the IP destination
					msg.actions.append(of.ofp_action_output(port=outport)) # Add an action which sets the Outport
		
					self.connection.send(msg)

		else:
			log.info("Unknown Packet type: %s" % packet.type)
			return
		return


def launch(loadbalancer, servers):
	# Color-coding and pretty-printing the log output
	pox.log.color.launch()
	pox.log.launch(format="[@@@bold@@@level%(name)-23s@@@reset] " +
						  "@@@bold%(message)s@@@normal")
	log.info("Loading Simple Load Balancer module:\n\n-----------------------------------CONFIG----------------------------------\n")
	server_ips = servers.replace(","," ").split()
	server_ips = [IPAddr(x) for x in server_ips]
	loadbalancer_ip = IPAddr(loadbalancer)
	log.info("Loadbalancer IP: %s" % loadbalancer_ip)
	log.info("Backend Server IPs: %s\n\n---------------------------------------------------------------------------\n\n" % ', '.join(str(ip) for ip in server_ips))
	core.registerNew(SimpleLoadBalancer, loadbalancer_ip, server_ips)
