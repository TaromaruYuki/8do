DATA = {
    "instructions": {
        "LDR": {"size": 2, "opcode": {"RI": 0x2B, "RA": 0x23, "RR": 0x34, "RP": 0x52}},
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
        "LDO": {"size": 2, "opcode": {"RA": 0x20, "RR": 0x31}},
        "NOP": {"size": 0, "opcode": 0x00},
        "PSH": {"size": 1, "opcode": {"A": 0x4C, "R": 0x3D, "I": 0x74}},
        "POP": {"size": 1, "opcode": {"A": 0x30, "R": 0x41}},
        "JSR": {"size": 1, "opcode": {"A": 0xEF}},
        "RET": {"size": 0, "opcode": 0xEB},
        "BII": {"size": 1, "opcode": {"A": 0xD4}},
        "BNI": {"size": 1, "opcode": {"A": 0xD9}},
        "CLI": {"size": 0, "opcode": 0xD8},
        "SEI": {"size": 0, "opcode": 0xE1},
        "RTI": {"size": 0, "opcode": 0x0F}
    },
    "registers": ["RA", "RB", "RC", "RD", "RO"],
    "keywords": ["ORG", "WORD", "BYTE", "ASCII", "ASCIIZ"]
}

import sys
from enum import Enum
from dataclasses import dataclass
from string import hexdigits, ascii_letters

class TokenType(Enum):
    INT = 0
    IDENTIFIER = 1
    KEYWORD = 2
    REGISTER = 3
    LABEL = 4
    DIRECTIVE = 5
    INSTRUCTION = 6
    ADDRESS = 7
    POINTER = 8
    REFERENCE = 9
    OFFSET = 10
    STRING = 11

@dataclass
class Token:
    type_: TokenType
    value: str

class Lexer:
    def __init__(self, text: str):
        self.text = text
        self.current_char = None
        self.position = -1
        self.__advance()

    def __advance(self):
        self.position += 1
        self.current_char = self.text[self.position] if self.position <= len(self.text) else None

    def tokenize(self):
        tokens = []

        while self.current_char is not None:
            if self.current_char == '0':
                self.__advance()
                if self.current_char is None or self.current_char not in "xX":
                    return None, "Expected 'x' or 'X'"
                
                self.__advance()
                
                tokens.append(self.__make_number())
            elif self.current_char == '$':
                self.__advance()
                tokens.append(self.__make_number())
            elif self.current_char == '.':
                self.__advance()
                token = self.__make_identifier_or_keyword()
                if token.type_ != TokenType.KEYWORD:
                    return None, f"Expected directive, not '{token.value}'"
                token.type_ = TokenType.DIRECTIVE

                tokens.append(token)
            elif self.current_char == ':':
                if len(tokens) == 0:
                    return None, "No label name given, only found ':'."

                name: Token = tokens[-1]
                
                if name.type_ != TokenType.IDENTIFIER:
                    return None, f"Expected Identifier, found '{name.value}'"

                name.type_ = TokenType.LABEL

                tokens[-1] = name
            elif self.current_char == '"':
                self.__advance()
                tokens.append(self.__make_string())
            else:
                self.__advance()

        return tokens, None

    def __make_number(self) -> Token:
        num = ""

        while self.current_char is not None and self.current_char in hexdigits:
            num += self.current_char
            self.__advance()

        return Token(TokenType.INT, num)

    def __make_identifier_or_keyword(self) -> Token:
        word = ""

        while self.current_char is not None and self.current_char in ascii_letters + "_":
            word += self.current_char
            self.__advance()

        if word in DATA['keywords']:
            return Token(TokenType.KEYWORD, word)
        else:
            return Token(TokenType.IDENTIFIER, word)
        
    def __make_string(self) -> Token:
        string = ""
        escaped = True

        while self.current_char is not None and not escaped:
            if escaped:
                string += self.current_char
                escaped = False
                continue

            if self.current_char == '"':
                break

            string += self.current_char

        return Token(TokenType.STRING)
    
if __name__ == "__main__":
    # print(sys.argv)
    # sys.argv.pop(0)

    with open("gen/hello_world.8do", "r") as f: # sys.argv[0]
        text = f.read()

    lexer = Lexer(text)
    print(lexer.tokenize())
