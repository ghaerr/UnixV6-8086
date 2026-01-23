/* PC keyboard interface constants */

#define KBSTATP     0x64    /* kbd controller status port(I) */
#define KBS_DIB     0x01    /* kbd data in buffer */
#define KBDATAP     0x60    /* kbd data port(I) */

#define NO          0

#define SHIFT       (1<<0)
#define CTL         (1<<1)
#define ALT         (1<<2)

#define CAPSLOCK    (1<<3)
#define NUMLOCK     (1<<4)
#define SCROLLLOCK  (1<<5)

#define E0ESC       (1<<6)

/* Special keycodes */
#define KEY_HOME    0xE0
#define KEY_END     0xE1
#define KEY_UP      0xE2
#define KEY_DN      0xE3
#define KEY_LF      0xE4
#define KEY_RT      0xE5
#define KEY_PGUP    0xE6
#define KEY_PGDN    0xE7
#define KEY_INS     0xE8
#define KEY_DEL     0xE9

/* C('A') == Control-A */
#define C(x) (x - '@')

typedef unsigned char uchar;
typedef unsigned int uint;

static uchar shiftcode[256];
static uchar togglecode[256];
static uchar normalmap[256] =
{
    NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  /* 0x00 */
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  /* 0x10 */
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  /* 0x20 */
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  /* 0x30 */
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  /* 0x40 */
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   /* 0x50 */
};

static uchar shiftmap[256] =
{
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  /* 0x00 */
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  /* 0x10 */
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  /* 0x20 */
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  /* 0x30 */
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  /* 0x40 */
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   /* 0x50 */
};

static uchar ctlmap[256] =
{
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
    C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
    C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
    NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
    C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
};

void kbd_init(void)
{
    shiftcode[0x1D] = CTL;
    shiftcode[0x2A] = SHIFT;
    shiftcode[0x36] = SHIFT;
    shiftcode[0x38] = ALT;
    shiftcode[0x9D] = CTL;
    shiftcode[0xB8] = ALT;

    togglecode[0x3A] = CAPSLOCK;
    togglecode[0x45] = NUMLOCK;
    togglecode[0x46] = SCROLLLOCK;    

    normalmap[0x9C] = '\n';      /* KP_Enter */
    normalmap[0xB5] = '/';       /* KP_Div */
    normalmap[0xC8] = KEY_UP;
    normalmap[0xD0] = KEY_DN;
    normalmap[0xC9] = KEY_PGUP;
    normalmap[0xD1] = KEY_PGDN;
    normalmap[0xCB] = KEY_LF;
    normalmap[0xCD] = KEY_RT;
    normalmap[0x97] = KEY_HOME;
    normalmap[0xCF] = KEY_END;
    normalmap[0xD2] = KEY_INS;   
    normalmap[0xD3] = KEY_DEL;

    shiftmap[0x9C] = '\n';      /* KP_Enter */
    shiftmap[0xB5] = '/';       /* KP_Div */
    shiftmap[0xC8] = KEY_UP;    
    shiftmap[0xD0] = KEY_DN;
    shiftmap[0xC9] = KEY_PGUP;  
    shiftmap[0xD1] = KEY_PGDN;
    shiftmap[0xCB] = KEY_LF;    
    shiftmap[0xCD] = KEY_RT;
    shiftmap[0x97] = KEY_HOME;  
    shiftmap[0xCF] = KEY_END;
    shiftmap[0xD2] = KEY_INS;   
    shiftmap[0xD3] = KEY_DEL;  

    ctlmap[0x9C] = '\r';      /* KP_Enter */
    ctlmap[0xB5] = C('/');    /* KP_Div */
    ctlmap[0xC8] = KEY_UP;    
    ctlmap[0xD0] = KEY_DN;
    ctlmap[0xC9] = KEY_PGUP;  
    ctlmap[0xD1] = KEY_PGDN;
    ctlmap[0xCB] = KEY_LF;    
    ctlmap[0xCD] = KEY_RT;
    ctlmap[0x97] = KEY_HOME;  
    ctlmap[0xCF] = KEY_END;
    ctlmap[0xD2] = KEY_INS;   
    ctlmap[0xD3] = KEY_DEL;
}

extern uchar inportb(uint port);
int kbd_getc(void)
{
    static uint shift;
    static uchar *charcode[4] = {
        normalmap, shiftmap, ctlmap, ctlmap
    };
    uint st, data, c;

    st = inportb(KBSTATP);
    if((st & KBS_DIB) == 0)
        return -1;
    data = inportb(KBDATAP);

    if(data == 0xE0) {
        shift |= E0ESC;
        return 0;
    } else if(data & 0x80) {
        /* Key released */
        data = (shift & E0ESC ? data : data & 0x7f);
        shift &= ~(shiftcode[data] | E0ESC);
        return 0;
    } else if(shift & E0ESC) {
        /* Last character was an E0 escape; or with 0x80 */
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];
    if(shift & CAPSLOCK) {
        if('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    return c;
}

extern void klrxintr(void);
void kbdintr(void)
{
    klrxintr();
}
