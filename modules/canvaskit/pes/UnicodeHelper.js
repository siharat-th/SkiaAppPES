const THAI_KO_KAI           = 0xe01;
const THAI_KHO_KHAI         = 0xe02;
const THAI_KHO_KHUAT        = 0xe03;
const THAI_KHO_KHWAI        = 0xe04;
const THAI_KHO_KHON         = 0xe05;
const THAI_KHO_RAKHANG      = 0xe06;
const THAI_NGO_NGU          = 0xe07;
const THAI_CHO_CHAN         = 0xe08;
const THAI_CHO_CHING        = 0xe09;
const THAI_CHO_CHANG        = 0xe0a;
const THAI_SO_SO            = 0xe0b;
const THAI_CHO_CHOE         = 0xe0c;
const THAI_YO_YING          = 0xe0d;
const THAI_DO_CHADA         = 0xe0e;
const THAI_TO_PATAK         = 0xe0f;
const THAI_THO_THAN         = 0xe10;
const THAI_THO_NANGMONTHO   = 0xe11;
const THAI_THO_PHUTHAO      = 0xe12;
const THAI_NO_NEN           = 0xe13;
const THAI_DO_DEK           = 0xe14;
const THAI_TO_TAO           = 0xe15;
const THAI_THO_THUNG        = 0xe16;
const THAI_THO_THAHAN       = 0xe17;
const THAI_THO_THONG        = 0xe18;
const THAI_NO_NU            = 0xe19;
const THAI_BO_BAIMAI        = 0xe1a;
const THAI_PO_PLA           = 0xe1b;
const THAI_PHO_PHUNG        = 0xe1c;
const THAI_FO_FA            = 0xe1d;
const THAI_PHO_PHAN         = 0xe1e;
const THAI_FO_FAN           = 0xe1f;
const THAI_PHO_SAMPHAO      = 0xe20;
const THAI_MO_MA            = 0xe21;
const THAI_YO_YAK           = 0xe22;
const THAI_RO_RUA           = 0xe23;
const THAI_RU               = 0xe24;
const THAI_LO_LING          = 0xe25;
const THAI_LU               = 0xe26;
const THAI_WO_WAEN          = 0xe27;
const THAI_SO_SALA          = 0xe28;
const THAI_SO_RUSI          = 0xe29;
const THAI_SO_SUA           = 0xe2a;
const THAI_HO_HIP           = 0xe2b;
const THAI_LO_CHULA         = 0xe2c;
const THAI_O_ANG            = 0xe2d;
const THAI_HO_NOKHUK        = 0xe2e;

const THAI_PAIYANNOI        = 0xe2f;

const THAI_SARA_A           = 0xe30;
const THAI_MAI_HAN_AKAT     = 0xe31;
const THAI_SARA_AA          = 0xe32;
const THAI_SARA_AM          = 0xe33;
const THAI_SARA_I           = 0xe34;
const THAI_SARA_II          = 0xe35;
const THAI_SARA_UE          = 0xe36;
const THAI_SARA_UEE         = 0xe37;
const THAI_SARA_U           = 0xe38;
const THAI_SARA_UU          = 0xe39;
const THAI_PHINTHU          = 0xe3a;

const THAI_SYMBOL_BAHT      = 0xe3f;

const THAI_SARA_E           = 0xe40;
const THAI_SARA_AE          = 0xe41;
const THAI_SARA_O           = 0xe42;
const THAI_SARA_AI_MAIMUAN  = 0xe43;
const THAI_SARA_AI_MAIMALAI = 0xe44;
const THAI_LAKKHANGYAO      = 0xe45;
const THAI_MAIYAMOK         = 0xe46;
const THAI_MAITAIKHU        = 0xe47;

const THAI_MAI_EK           = 0xe48;
const THAI_MAI_THO          = 0xe49;
const THAI_MAI_TRI          = 0xe4a;
const THAI_MAI_CHATTAWA     = 0xe4b;

const THAI_THANTHAKHAT      = 0xe4c;
const THAI_NIKHAHIT         = 0xe4d;
const THAI_YAMAKKAN         = 0xe4e;

const THAI_FONGMAN          = 0xe4f;

const THAI_DIGIT_ZERO       = 0xe50;
const THAI_DIGIT_ONE        = 0xe51;
const THAI_DIGIT_TWO        = 0xe52;
const THAI_DIGIT_THREE      = 0xe53;
const THAI_DIGIT_FOUR       = 0xe54;
const THAI_DIGIT_FIVE       = 0xe55;
const THAI_DIGIT_SIX        = 0xe56;
const THAI_DIGIT_SEVEN      = 0xe57;
const THAI_DIGIT_EIGHT      = 0xe58;
const THAI_DIGIT_NINE       = 0xe59;

const THAI_ANGKHANKHU       = 0xe5a;
const THAI_KHOMUT           = 0xe5b;

// Win PUA
const THAI_THO_THAN_NONE_TAIL         = 0xf700;
const THAI_YO_YING_NONE_TAIL          = 0xf70f;

// return true if c is thai letter
function th_isThaiLetter(c)
{
    return c>=THAI_KO_KAI && c<=THAI_KHOMUT;
}

// return true if c is thai consonant
function th_isConsonant(c)
{
    return c>=THAI_KO_KAI && c<=THAI_HO_NOKHUK; // ก ข ... ฮ
}

// return true if c is thai leading vowel
function th_isLeadingvowel(c)
{
    return c>=THAI_SARA_E && c<=THAI_SARA_AI_MAIMALAI; // เ แ โ ใ ไ
}

// return true if c is thai above vowel
function th_isAboveVowel(c)
{
    return c>=THAI_SARA_I && c<=THAI_SARA_UEE;  //
}

// return true if c is thai lower vowel
function th_isBelowVowel(c)
{
    return c>=THAI_SARA_U && c<=THAI_PHINTHU;  //
}

// return true if c is thai tone
function th_isTone(c)
{
    return c>=THAI_MAI_EK && c<=THAI_MAI_CHATTAWA; //
}

function th_isNormalConsonantAndDiacritic(c)
{
    return (c>=THAI_KO_KAI && c<=THAI_CHO_CHOE) || (c>=THAI_THO_NANGMONTHO && c<=THAI_BO_BAIMAI) ||
            c==THAI_PHO_PHUNG || c==THAI_PHO_PHAN || (c>=THAI_PHO_SAMPHAO && c<=THAI_PAIYANNOI);
}

function th_isAscenderConsonant(c)            // Ascender consonants (ป, ฝ, ฟ)
{
    return c==THAI_PO_PLA || c==THAI_FO_FA || c==THAI_FO_FAN;
}

function th_isDescenderConsonant(c)           // Strict Descender consonants (ฎ, ฏ)
{
    return c==THAI_DO_CHADA || c==THAI_TO_PATAK;
}

function th_isRemovableDescenderConsonant(c)  // Removable descender consonants (ญ, ฐ)
{
    return c==THAI_YO_YING || c==THAI_THO_THAN;
}

function th_isAboveVowelAndDiacritic(c)       // (◌ั, ◌ิ, ◌ี, ◌ึ, ◌ื, ◌็, ◌ํ)
{
    return c==THAI_MAI_HAN_AKAT || th_isAboveVowel(c) || c==THAI_MAITAIKHU || c==THAI_NIKHAHIT;
}

function th_isBelowVowelAndDiacritic(c)       // (◌ุ, ◌ู, ◌ฺ)
{
    return th_isBelowVowel(c);
}

function th_isToneAndDiacritic(c)             // Above tone/diacritic ( ่, ้, ๊, ๋, ์, ◌๎)
{
    return th_isTone(c) || c==THAI_THANTHAKHAT || c==THAI_YAMAKKAN;
}
