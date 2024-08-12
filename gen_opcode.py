while True:
    result = 0
    
    for char in input("Enter the opcode: "):
        result += ord(char)
        
    if result == 0: continue
        
    print(f"Result: {hex(result & 0xFF).capitalize().replace('X', 'x')}")