DATA = {
    "instructions": {
        "LDR": {"size": 2, "opcode": {"RI": 0x2B, "RA": 0x23, "RR": 0x34}},
        "STR": {"size": 2, "opcode": {"RA": 0x3A}},
        "ADD": {"size": 2, "opcode": {"RI": 0x12, "RA": 0x0A, "RR": 0x1B}},
        "SUB": {"size": 2, "opcode": {"RI": 0x33, "RA": 0x4B, "RR": 0x3C}},
        "JMP": {"size": 1, "opcode": {"A": 0xE7}},
        "BIZ": {"size": 1, "opcode": {"A": 0xB0}},
        "BNZ": {"size": 1, "opcode": {"A": 0xB1}},
        "BIN": {"size": 1, "opcode": {"A": 0xB2}},
        "BNN": {"size": 1, "opcode": {"A": 0xB3}},
        "BIC": {"size": 1, "opcode": {"A": 0xB4}},
        "BNC": {"size": 1, "opcode": {"A": 0xB5}},
        "BIO": {"size": 1, "opcode": {"A": 0xB6}},
        "BNO": {"size": 1, "opcode": {"A": 0xB7}},
        "BIL": {"size": 1, "opcode": {"A": 0xB8}},
        "BNL": {"size": 1, "opcode": {"A": 0xB9}},
        "BIG": {"size": 1, "opcode": {"A": 0xBA}},
        "BNG": {"size": 1, "opcode": {"A": 0xBB}},
        "SBL": {"size": 1, "opcode": {"A": 0x22, "R": 0x01}},
        "SBR": {"size": 1, "opcode": {"A": 0x28, "R": 0x39}},
        "ROL": {"size": 1, "opcode": {"A": 0x2E, "R": 0x3F}},
        "ROR": {"size": 1, "opcode": {"A": 0x54, "R": 0x45}},
        "CLC": {"size": 0, "opcode": 0xD2},
        "INC": {"size": 1, "opcode": {"A": 0x3B, "R": 0x2C}},
        "DEC": {"size": 1, "opcode": {"A": 0x0D, "R": 0x1E}},
        "HLT": {"size": 0, "opcode": 0xE8},
        "CMP": {"size": 2, "opcode": {"RI": 0x29, "RA": 0x21, "RR": 0x32}},
        "LDO": {"size": 2, "opcode": {"RA": 0xDF}},
        "NOP": {"size": 0, "opcode": 0x00},
        "PSH": {"size": 1, "opcode": {"A": 0x4C, "R": 0x3D, "I": 0x74}},
        "POP": {"size": 1, "opcode": {"A": 0x30, "R": 0x41}},
        "JSR": {"size": 1, "opcode": {"A": 0xEF}},
        "RET": {"size": 0, "opcode": 0xEB},
    },
    "registers": ["RA", "RB", "RC", "RD", "RO"],
    "keywords": [".ORG", ".WORD", ".BYTE", ".ASCII", ".ASCIIZ"]
}

def assemble(file):
    with open(file, "r") as f:
        text = f.read()
    
    result = [0x00 for _ in range(0x3FFFF + 1)]
    addr = 0x00000
    comment = False
    org = False
    instruction = False
    iword = False
    ibyte = False
    ascii_ = False
    z = False
    name = ""
    new = []
    nametable = {}
    key = ""
    temp = None
    
    for line in text.split("\n"):
        line += "\n"
        for word in line.split(" "):
            if word == "" or word == "\n" or word == "\t":
                continue
        
            if word[0] == ";":
                comment = True
        
            if comment:
                if word[-1] == "\n":
                    comment = False
                continue
        
            new.append(word.replace("\n", "").replace("\t", ""))
            
    new.append(" ")
        
    for word in new:
        if org:
            addr = int(word.replace("$", ""), 16)
            org = False
            continue
        
        if iword:
            if word[0] == "$":
                value = int(word.replace("$", ""), 16)
                result[addr] = (value >> 8)
                result[addr + 1] = value & 0xFF
            else:
                if nametable.get(word):
                    value = nametable[word]
                    result[addr] = (value >> 8)
                    result[addr + 1] = value & 0xFF
                else:
                    result[addr] = word
                    result[addr + 1] = None
            addr += 2
            iword = False
            continue
        
        if ibyte:
            value = int(word.replace("0x", ""), 16)
            result[addr] = value
            addr += 1
            ibyte = False
            continue
        
        if ascii_:
            escaped = False
            escape_seqs = {"n": "\n", "\\": "\\", "\"": "\""}
            
            if word[0] == '"':
                word = word[1:]
                
            for char in word:
                if not escaped and char == "\"":
                    if z:
                        result[addr] = 0x00
                        addr += 1
                        z = False
                    ascii_ = False
                    break
                
                if escaped:
                    result[addr] = ord(escape_seqs[char])
                    addr += 1
                    continue
                
                result[addr] = ord(char)
                addr += 1

            if ascii_:
                result[addr] = ord(" ")
                addr += 1
            
            continue
        
        if instruction:
            if (word.upper() in DATA["instructions"] and not key) or word == " ":
                result[addr] = DATA["instructions"][name]["opcode"]
                result[addr + 1] = 0x3F
                addr += 2
                name = word.upper()
                continue
            
            if word in DATA["keywords"]:
                result[addr] = DATA["instructions"][name]["opcode"]
                result[addr + 1] = 0x3F
                addr += 2
                
                if word.upper() == ".ORG":
                    instruction = False
                    org = True
                    continue
                elif word.upper() == ".WORD":
                    instruction = False
                    iword = True
                    continue
                elif word.upper() == ".BYTE":
                    instruction = False
                    ibyte = True
                    continue
                elif word.upper() == ".ASCII":
                    instruction = False
                    ascii_ = True
                    continue
                elif word.upper() == ".ASCIIZ":
                    instruction = False
                    ascii_ = True
                    z = True
                    continue

            if word[-1] == ":":
                result[addr] = DATA["instructions"][name]["opcode"]
                result[addr + 1] = 0x3F
                addr += 2

                nametable[word.replace(":", "")] = addr
                instruction = False
                continue
            
            if word.upper().replace(",", "").replace(" ", "") in DATA["registers"]:
                key += "R"
                
                if len(key) == 1 and "," in word.upper():
                    temp = word.upper()
                    continue
                elif len(key) == 1:
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg = lookup[word[1]]
                    
                    result[addr + 1] = reg
                    
                    addr += 2
                    key = ""
                    instruction = False
                    continue
                elif len(key) == 2:
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg1 = lookup[temp[1]]
                    reg2 = lookup[word[1]]

                    result[addr + 1] = (reg2 << 3) | reg1
                    
                    addr += 2
                    key = ""
                    temp = None
                    instruction = False
                    continue
            elif word.upper()[0] == "$":
                key += "A"
                
                if len(key) == 2:
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg1 = lookup[temp[1]]

                    result[addr + 1] = reg1

                    value = int(word.replace("$", ""), 16)

                    if value > 0xFFFF:
                        result[addr + 1] |= (value & 0x30000) >> 10;
                    
                    result[addr + 2] = (value >> 8) & 0xFF
                    result[addr + 3] = value & 0xFF
                    
                    addr += 4
                    key = ""
                    temp = None
                    instruction = False
                    continue
                
                result[addr] = DATA["instructions"][name]["opcode"][key]
                result[addr + 1] = 0x3F
                
                value = int(word.replace("$", ""), 16)
                
                if value > 0xFFFF:
                    result[addr + 1] |= (value & 0x30000) >> 10;
                
                result[addr + 2] = (value >> 8)
                result[addr + 3] = value & 0xFF
                
                addr += 4
                key = ""
                instruction = False
                continue
            elif "0x" in word:
                key += "I"
                
                if len(key) == 2:
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg1 = lookup[temp[1]]

                    result[addr + 1] = reg1

                    value = int(word.replace("0x", ""), 16)
                    
                    result[addr + 2] = value
                
                    addr += 3
                    key = ""
                    temp = None
                    instruction = False
                    continue
                
                result[addr] = DATA["instructions"][name]["opcode"][key]
                result[addr + 1] = 0x3F
                
                value = int(word.replace("0x", ""), 16)
                
                result[addr + 2] = value
                
                addr += 3
                key = ""
                instruction = False
                continue
            else:
                key += "A"
                
                if word in nametable:
                    if len(key) == 2:
                        result[addr] = DATA["instructions"][name]["opcode"][key]
                        
                        if temp:
                            lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                            reg1 = lookup[temp[1]]
                        
                            result[addr + 1] = reg1
                        else:
                            result[addr + 1] = 0x3F
                        
                        value = nametable[word]
                        
                        if value > 0xFFFF:
                            result[addr + 1] |= (value & 0x30000) >> 10;
                        
                        result[addr + 2] = (value >> 8) & 0xFF
                        result[addr + 3] = value & 0xFF
                        
                        addr += 4
                        key = ""
                        temp = None
                        instruction = False
                        continue
                    
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    result[addr + 1] = 0x3F
                    
                    value = nametable[word]
                    
                    result[addr + 2] = (value >> 8)
                    result[addr + 3] = value & 0xFF
                    
                    addr += 4
                    key = ""
                    instruction = False
                    continue
        
                result[addr] = DATA["instructions"][name]["opcode"][key] # Ignore first breakpoint activation.
                        
                if temp:
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg1 = lookup[temp[1]]
                        
                    result[addr + 1] = reg1
                else:
                    result[addr + 1] = 0x3F
                    
                result[addr + 2] = word
                result[addr + 3] = None
                
                addr += 4
                key = ""
                instruction = False
                continue

        if word.upper() == ".ORG":
            org = True
            continue
        elif word.upper() == ".WORD":
            iword = True
            continue
        elif word.upper() == ".BYTE":
            ibyte = True
            continue
        elif word.upper() == ".ASCII":
            ascii_ = True
            continue
        elif word.upper() == ".ASCIIZ":
            ascii_ = True
            z = True
            continue
        elif word[-1] == ":":
            nametable[word.replace(":", "")] = addr
            continue
        
        if word.upper() in DATA["instructions"]:
            instruction = True
            name = word.upper()
            continue

        print(f"\"{word}\" is invalid.")
        exit(1)
        
    for i, byte in enumerate(result):
        if isinstance(byte, str):
            if not (byte in nametable):
                print(f"Error: {word} not found in nametable")
                exit(1)

            value = nametable[byte]

            if value > 0xFFFF:
                result[i - 1] |= (value & 0x30000) >> 10;
                
            result[i] = (value >> 8) & 0xFF
            result[i + 1] = (value & 0xFF)
            continue
        elif byte is None:
            exit(1)
    
    with open(file.replace(".8do", ".bin"), "wb") as f:
        f.write(bytes(result))

if __name__ == "__main__":
    import sys
    assemble(sys.argv[1])