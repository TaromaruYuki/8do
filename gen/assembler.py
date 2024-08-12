import string

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
        "CMP": {"size": 2, "opcode": {"RI": 0x29, "RA": 0x21, "RR": 0x32}}
    },
    "registers": ["RA", "RB", "RC", "RD", "RO"],
    "keywords": ["ORG", "WORD", "BYTE"]
}

DATA["keywords"].extend(DATA['instructions'].keys())
    
def assemble(file):
    with open(file, "r") as f:
        text = f.read()
    
    result = [0xE8 for _ in range(0xFFFF)]
    addr = 0x0000
    comment = False
    org = False
    instruction = False
    iword = False
    ibyte = False
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
        
    for word in new:
        if org:
            addr = int(word.replace("$", ""), 16)
            org = False
            continue
        
        if iword:
            value = int(word.replace("$", ""), 16)
            result[addr] = (value & 0xFF00) >> 8
            result[addr + 1] = value & 0xFF
            addr += 2
            iword = False
            continue
        
        if ibyte:
            value = int(word.replace("0x", ""), 16)
            result[addr] = value
            addr += 1
            ibyte = False
            continue
        
        if instruction:
            if word.upper() in DATA["instructions"] and not key:
                result[addr] = DATA["instructions"][name]["opcode"]
                addr += 1
                name = word.upper()
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

                    result[addr + 1] = (reg1 << 3) | reg2
                    
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
                    
                    result[addr + 2] = (value & 0xFF00) >> 8
                    result[addr + 3] = value & 0xFF
                    
                    addr += 4
                    key = ""
                    temp = None
                    instruction = False
                    continue
                
                result[addr] = DATA["instructions"][name]["opcode"][key]
                result[addr + 1] = 0xFF
                
                value = int(word.replace("$", ""), 16)
                
                result[addr + 2] = (value & 0xFF00) >> 8
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
                
                result[addr] = DATA["instructions"][name]["opcode"]["key"]
                result[addr + 1] = 0xFF
                
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
                            result[addr + 1] = 0xFF
                        
                        value = nametable[word]
                        
                        result[addr + 2] = (value & 0xFF00) >> 8
                        result[addr + 3] = value & 0xFF
                        
                        addr += 4
                        key = ""
                        temp = None
                        instruction = False
                        continue
                    
                    result[addr] = DATA["instructions"][name]["opcode"][key]
                    result[addr + 1] = 0xFF
                    
                    value = nametable[word]
                    
                    result[addr + 2] = (value & 0xFF00) >> 8
                    result[addr + 3] = value & 0xFF
                    
                    addr += 4
                    key = ""
                    instruction = False
                    continue
        
                result[addr] = DATA["instructions"][name]["opcode"][key]
                        
                if temp:
                    lookup = {"A": 0, "B": 1, "C": 2, "D": 3, "O": 4}
                    reg1 = lookup[temp[1]]
                        
                    result[addr + 1] = reg1
                else:
                    result[addr + 1] = 0xFF
                    
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
        elif word[-1] == ":":
            nametable[word.replace(":", "")] = addr + 1
            continue
        
        if word.upper() in DATA["instructions"]:
            instruction = True
            name = word.upper()
            continue
    
    with open("gen/test.bin", "wb") as f:
        for i, byte in enumerate(result):
            if isinstance(byte, str):
                if not (byte in nametable):
                    print(f"Error: {word} not found in nametable")
                    exit(1)

                value = nametable[byte]
                
                f.write(((value & 0xFF00) >> 8).to_bytes())
                f.write((value & 0xFF).to_bytes())
                result[i + 1] = (value & 0xFF)
                continue
            elif byte is None:
                exit(1)
                
            f.write(byte.to_bytes())

assemble("gen/test.8do")

#if __name__ != "__main__":
#    # file = input("File name: ")
#    file = "gen/test.8do"
#    out = [0x00 for _ in range(0x10000)]
#    
#    lexer = Lexer(file)
#    tokens = lexer.tokenize()
#    
#    print("\n".join(map(str, tokens)))
