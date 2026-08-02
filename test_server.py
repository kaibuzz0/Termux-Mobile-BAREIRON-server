#!/usr/bin/env python3
"""
BAREIRON SERVER TESTER
Usage: python3 test_server.py [HOST] [PORT]
Default: python3 test_server.py 127.0.0.1 25565
"""

import socket
import struct
import sys
import json
import time

def read_varint(sock):
    """Read a Minecraft varint from socket."""
    result = 0
    for i in range(5):
        byte = sock.recv(1)
        if not byte:
            return None
        val = byte[0]
        result |= (val & 0x7F) << (7 * i)
        if not (val & 0x80):
            return result
    return None

def write_varint(value):
    """Encode value as varint bytes."""
    result = bytearray()
    while True:
        byte = value & 0x7F
        value >>= 7
        if value:
            byte |= 0x80
        result.append(byte)
        if not value:
            break
    return bytes(result)

def test_server(host="127.0.0.1", port=25565):
    print(f"\n═════════════════════════════════════════════════════════════")
    print(f"  🧟 BAREIRON SERVER TEST")
    print(f"  Target: {host}:{port}")
    print(f"═════════════════════════════════════════════════════════════")
    
    # Step 1: TCP Connection
    print("\n[1/3] Connecting to server...")
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((host, port))
        print("      ✅ TCP connection established")
    except Exception as e:
        print(f"      ❌ Connection failed: {e}")
        print(f"\n      Is the server running? Try:")
        print(f"        ./quickstart.sh")
        return False
    
    # Step 2: Minecraft Handshake
    print("\n[2/3] Sending Minecraft handshake (protocol 772)...")
    try:
        # Handshake packet: varint length, packet ID 0x00, protocol varint, address string, port short, next state varint (1 = status)
        address = b"\x00"  # Empty server address
        protocol = write_varint(772)  # 1.21.8
        addr_len = write_varint(0)    # Empty string length
        port = struct.pack(">H", 25565)
        next_state = write_varint(1)
        
        handshake_data = protocol + addr_len + port + next_state
        handshake_packet = write_varint(len(handshake_data) + 1) + b"\x00" + handshake_data
        
        sock.sendall(handshake_packet)
        
        # Status request
        status_req = write_varint(1) + b"\x00"
        sock.sendall(status_req)
        print("      ✅ Handshake + status request sent")
    except Exception as e:
        print(f"      ❌ Handshake failed: {e}")
        sock.close()
        return False
    
    # Step 3: Read Response
    print("\n[3/3] Reading server status...")
    try:
        length = read_varint(sock)
        if length is None:
            print("      ❌ No response from server")
            sock.close()
            return False
        
        packet_id = read_varint(sock)
        if packet_id is None:
            print("      ❌ Invalid packet")
            sock.close()
            return False
        
        json_len = read_varint(sock)
        if json_len is None or json_len > 65535:
            print("      ❌ Invalid JSON length")
            sock.close()
            return False
        
        json_data = b""
        while len(json_data) < json_len:
            chunk = sock.recv(json_len - len(json_data))
            if not chunk:
                break
            json_data += chunk
        
        response = json.loads(json_data.decode('utf-8'))
        
        print(f"      ✅ Server responded!")
        print(f"\n      📋 Server Info:")
        print(f"         Version: {response.get('version', {}).get('name', 'Unknown')}")
        print(f"         Protocol: {response.get('version', {}).get('protocol', 'Unknown')}")
        
        desc = response.get('description', {})
        if isinstance(desc, dict):
            print(f"         MOTD: {desc.get('text', 'No MOTD')}")
        else:
            print(f"         MOTD: {desc}")
        
        players_info = response.get('players', {})
        if players_info:
            print(f"         Players: {players_info.get('online', '?')}/{players_info.get('max', '?')}")
        
        print(f"\n      ✅ ALL TESTS PASSED")
        print(f"═════════════════════════════════════════════════════════════")
        
        sock.close()
        return True
        
    except json.JSONDecodeError as e:
        print(f"      ❌ Invalid JSON response: {e}")
        sock.close()
        return False
    except Exception as e:
        print(f"      ❌ Error reading response: {e}")
        sock.close()
        return False

if __name__ == "__main__":
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 25565
    
    success = test_server(host, port)
    sys.exit(0 if success else 1)
