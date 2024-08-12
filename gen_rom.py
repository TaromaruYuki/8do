rom = [0xE8 for _ in range(0x10000)]

# Multiplication?
"""
$0100: Factor2 / Result Hi
$0101: Result Lo
== == == == == == == == == == == ==
$0216: loop
$0220: noadd
"""


rom[0x0000] = 0x02
rom[0x0001] = 0x00

rom[0x0200] = 0x2B         # LDR  |
rom[0x0201] = 0b00_000_000 # RA,  | LDR RA, 0xD0
rom[0x0202] = 0xD0         # 0xD0 |

rom[0x0203] = 0x2B         # LDR  |
rom[0x0204] = 0b00_000_001 # RB,  | LDR RB, 0x07
rom[0x0205] = 0x07         # 0x07 |

rom[0x0206] = 0x39         # LSR  |
rom[0x0207] = 0b00_000_000 # RA   | LSR RA

rom[0x0208] = 0x3A         # STR  |
rom[0x0209] = 0b00_000_000 # RA,  | STR RA, $0101
rom[0x020A] = 0x01         # 0x01 |
rom[0x020B] = 0x01         # 0x01 |

rom[0x020C] = 0x3A         # STR  |
rom[0x020D] = 0b00_000_001 # RB,  | STR RB, $0100
rom[0x020E] = 0x01         # 0x01 |
rom[0x020F] = 0x00         # 0x00 |

rom[0x0210] = 0x2B         # LDR  |
rom[0x0211] = 0b00_000_000 # RA,  | LDR RA, 0x00
rom[0x0212] = 0x00         # 0x00 |

rom[0x0213] = 0x2B         # LDR  |
rom[0x0214] = 0b00_000_001 # RB,  | LDR RB, 0x08
rom[0x0215] = 0x08         # 0x08 |

rom[0x0216] = 0xB5         # BNC  |
rom[0x0217] = 0b11_111_111 #      | BNC noadd
rom[0x0218] = 0x02         #      | 
rom[0x0219] = 0x20         #      |

rom[0x021A] = 0xD2         # CLC  |
rom[0x021B] = 0b11_111_111 #      | CLC

rom[0x021C] = 0x0A         # ADD  |
rom[0x021D] = 0b00_000_000 # RA,  | ADD RA, $0100
rom[0x021E] = 0x01         # 0x01 |
rom[0x021F] = 0x00         # 0x00 |

rom[0x0220] = 0x45         # ROR  |
rom[0x0221] = 0b00_000_000 # RA   | ROR RA

rom[0x0222] = 0x45         # ROR  |
rom[0x0223] = 0b11_111_111 #      | ROR $0101
rom[0x0224] = 0x01         # 0x01 |
rom[0x0225] = 0x01         # 0x01 |

rom[0x0226] = 0x1E         # DEC  |
rom[0x0227] = 0b00_000_001 # RB   | DEC RB

rom[0x0228] = 0xB1         # BNZ  |
rom[0x0229] = 0b11_111_111 #      | BNZ loop
rom[0x022A] = 0x02         #      |
rom[0x022B] = 0x16         #      |

rom[0x022C] = 0x3A         # STR  |
rom[0x022D] = 0b00_000_000 # RA,  | STR RA, $0100
rom[0x022E] = 0x01         # 0x01 |
rom[0x022F] = 0x00         # 0x00 |

with open("rom.bin", "wb") as f:
    f.write(bytearray(rom))

