#include "coldplate.h"
#include <base64.hpp>

// Splash screens
const uint16_t cat_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0001, 0xF00F, 0x8000},
    {0x0003, 0xF81F, 0xC000},
    {0x0007, 0xFC3F, 0xC000},
    {0x0007, 0xFC3F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0003, 0xFE7F, 0xC000},
    {0x03E3, 0xFC3F, 0xC7C0},
    {0x07F1, 0xF83F, 0x8FE0},
    {0x07F8, 0x700E, 0x1FE0},
    {0x07F8, 0x0000, 0x3FE0},
    {0x0FFC, 0x07C0, 0x3FE0},
    {0x07FC, 0x1FF8, 0x3FE0},
    {0x07FC, 0x3FFC, 0x3FE0},
    {0x07FC, 0x7FFE, 0x3FE0},
    {0x03F8, 0xFFFF, 0x3FC0},
    {0x01F9, 0xFFFF, 0x9F80},
    {0x00F3, 0xFFFF, 0xCF00},
    {0x0007, 0xFFFF, 0xE000},
    {0x000F, 0xFFFF, 0xE000},
    {0x000F, 0xFFFF, 0xF000},
    {0x001F, 0xFFFF, 0xF000},
    {0x001F, 0xFFFF, 0xF800},
    {0x001F, 0xFFFF, 0xF800},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x007F, 0xFFFF, 0xFC00},
    {0x007F, 0xF00F, 0xFC00},
    {0x003F, 0xC003, 0xFC00},
    {0x003F, 0x0000, 0xFC00},
    {0x003E, 0x0000, 0x7C00},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
};
const uint16_t caution_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x0FF0, 0x0000},
    {0x0000, 0x0FF0, 0x0000},
    {0x0000, 0x1E78, 0x0000},
    {0x0000, 0x1E78, 0x0000},
    {0x0000, 0x3C3C, 0x0000},
    {0x0000, 0x3C3C, 0x0000},
    {0x0000, 0x781E, 0x0000},
    {0x0000, 0x781E, 0x0000},
    {0x0000, 0xF00F, 0x0000},
    {0x0000, 0xF00F, 0x0000},
    {0x0001, 0xE007, 0x8000},
    {0x0001, 0xE007, 0x8000},
    {0x0003, 0xC003, 0xC000},
    {0x0003, 0xC3C3, 0xC000},
    {0x0007, 0x87E1, 0xE000},
    {0x0007, 0x87E1, 0xE000},
    {0x000F, 0x07E0, 0xF000},
    {0x000F, 0x07E0, 0x7800},
    {0x001E, 0x07E0, 0x7800},
    {0x003C, 0x07E0, 0x3C00},
    {0x003C, 0x07E0, 0x3C00},
    {0x0078, 0x07E0, 0x1E00},
    {0x0078, 0x03C0, 0x1E00},
    {0x00F0, 0x03C0, 0x0F00},
    {0x00F0, 0x03C0, 0x0F00},
    {0x01E0, 0x03C0, 0x0780},
    {0x01E0, 0x03C0, 0x0780},
    {0x03C0, 0x0180, 0x03C0},
    {0x03C0, 0x0180, 0x03C0},
    {0x0780, 0x0180, 0x01E0},
    {0x0780, 0x0180, 0x01E0},
    {0x0F00, 0x0000, 0x00F0},
    {0x0F00, 0x03C0, 0x00F0},
    {0x1E00, 0x07E0, 0x0078},
    {0x1E00, 0x07E0, 0x0078},
    {0x3C00, 0x07E0, 0x003C},
    {0x3C00, 0x07E0, 0x003C},
    {0x7800, 0x07E0, 0x001E},
    {0x7800, 0x03C0, 0x001E},
    {0xF000, 0x0000, 0x000F},
    {0xF000, 0x0000, 0x000F},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0x7FFF, 0xFFFF, 0xFFFE},
};
const uint16_t snowflake_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0180, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x1188, 0x0000},
    {0x0000, 0x1DB8, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x00E4, 0x03C0, 0x2780},
    {0x01E4, 0x0180, 0x2780},
    {0x01EC, 0x0DB0, 0x3780},
    {0x01EC, 0x07E0, 0x3780},
    {0x001D, 0x83C1, 0xB800},
    {0x007D, 0x8181, 0xBE00},
    {0x01FF, 0x8001, 0xFF80},
    {0x0007, 0x9CB9, 0xE000},
    {0x001F, 0x9FF9, 0xF800},
    {0x001F, 0x9FF9, 0xF800},
    {0x0000, 0x7BDE, 0x0000},
    {0x0000, 0x799E, 0x0000},
    {0x0C03, 0xF99F, 0xC030},
    {0x0443, 0xFDBF, 0xC220},
    {0x0663, 0x8FF1, 0xC660},
    {0x3331, 0xC7E3, 0x8CCC},
    {0x7FF9, 0xFFFF, 0x9FFE},
    {0x7FFB, 0xFFFF, 0xDFFE},
    {0x3331, 0xC7E3, 0x8CCC},
    {0x0663, 0x8FF1, 0xC660},
    {0x0443, 0xFDBF, 0xC220},
    {0x0C03, 0xF99F, 0xC030},
    {0x0000, 0x799E, 0x0000},
    {0x0000, 0x7BDE, 0x0000},
    {0x001F, 0x9FF9, 0xF800},
    {0x001F, 0x9FF9, 0xF800},
    {0x0007, 0x9CB9, 0xE000},
    {0x01FF, 0x8001, 0xFF80},
    {0x007D, 0x8181, 0xBE00},
    {0x001D, 0x83C1, 0xB800},
    {0x01EC, 0x07E0, 0x3780},
    {0x01EC, 0x0DB0, 0x3780},
    {0x01E4, 0x0180, 0x2780},
    {0x01E4, 0x03C0, 0x2780},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x1DB8, 0x0000},
    {0x0000, 0x1188, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x0180, 0x0000},
    {0x0000, 0x0000, 0x0000},
};
const uint16_t ultrasonic_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0003, 0x8000},
    {0x0000, 0x0003, 0xE000},
    {0x0000, 0x0001, 0xF800},
    {0x0000, 0x0000, 0x3C00},
    {0x0000, 0x0004, 0x0F00},
    {0x0000, 0x000F, 0x0780},
    {0x0000, 0x0007, 0xC3C0},
    {0x0000, 0x0001, 0xE1C0},
    {0x0000, 0x0000, 0xF0E0},
    {0x0000, 0x000C, 0x3870},
    {0x0000, 0x001E, 0x1C70},
    {0x0000, 0x3C0F, 0x8E38},
    {0x0000, 0x7F83, 0xC718},
    {0x0000, 0xFFC1, 0xC71C},
    {0x0000, 0xE1E0, 0xE398},
    {0x0000, 0xE078, 0x7380},
    {0x0000, 0xE03C, 0x7000},
    {0x0000, 0x601E, 0x0000},
    {0x0000, 0x600E, 0x0000},
    {0x0000, 0x6007, 0x0000},
    {0x0000, 0xE003, 0x8000},
    {0x0001, 0xC003, 0x8000},
    {0x0003, 0xC001, 0xC000},
    {0x0007, 0x9801, 0xC000},
    {0x000F, 0x3801, 0xC000},
    {0x001E, 0x79FB, 0xC000},
    {0x003C, 0x63FF, 0x8000},
    {0x0078, 0x079E, 0x0000},
    {0x00F0, 0x0E00, 0x0000},
    {0x01C0, 0x1C00, 0x0000},
    {0x0180, 0x3800, 0x0000},
    {0x0380, 0x7000, 0x0000},
    {0x0381, 0xE000, 0x0000},
    {0x0383, 0xC000, 0x0000},
    {0x0387, 0x8000, 0x0000},
    {0x03FF, 0x0000, 0x0000},
    {0x03FE, 0x0000, 0x0000},
    {0x0FE0, 0x0000, 0x0000},
    {0x1E00, 0x0000, 0x0000},
    {0x1C00, 0x0000, 0x0000},
    {0x1800, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
};

// Kmeans centroids
const uint32_t centroids[KMEANS_CENTROIDS][KMEANS_DIMENSIONALITY] PROGMEM = {
{0x30EE8AEF, 0x3176E609, 0x31B51643, 0x3261F55C, 0x32D8088E, 0x32ADF667, 0x326AE1CA, 0x3341A915, 
0x32C90C99, 0x31AC0755, 0x3279187B, 0x3247CCA3, 0x32D48774, 0x32A5675D, 0x317F6FDC, 0x31FCD726, 
0x322607C1, 0x32A05835, 0x32A2AE83, 0x322763BB, 0x326BD2C4, 0x32087C0D, 0x32A337C3, 0x32E59848, 
0x3271009E, 0x3378548A, 0x32D61470, 0x327E18C2, 0x306B14F3, 0x30C391F9, 0x2FD72FC1, 0x2FD0B7FE, 
0x31874EF4, 0x31F17900, 0x33B3FFC2, 0x341E5A09, 0x34072AD4, 0x349EC488, 0x34FCBC44, 0x3516B727, 
0x34A9E149, 0x33239D3E, 0x348BD9F0, 0x3443AAC8, 0x334C64F6, 0x3458BCA0, 0x3453CA8F, 0x333F4D30, 
0x33C233C1, 0x34090064, 0x34C36C04, 0x33C3095A, 0x33C7441B, 0x3393AC7E, 0x34E4C0E5, 0x34E12AA0, 
0x3452E643, 0x3551EE2F, 0x3350977C, 0x32AD6FF4, 0x31927B63, 0x30A969D9, 0x301CCFA8, 0x2F444998, 
0x3211F34A, 0x33802BA6, 0x35059C78, 0x35C13D8D, 0x36A2F35E, 0x360E63AC, 0x368CB336, 0x3724EEEA, 
0x35B870FF, 0x3474FAA6, 0x35772D05, 0x35FFD080, 0x3621E0D2, 0x366DD554, 0x34BDEBD6, 0x35C8753B, 
0x34CE0D08, 0x351057A0, 0x35A3F2DC, 0x35CE969A, 0x3542A182, 0x351E72CC, 0x3562619D, 0x35AD58A1, 
0x367BEE09, 0x368E5795, 0x350B1943, 0x33763EB1, 0x32F55839, 0x31B2393E, 0x30AA2776, 0x2F755259, 
0x31A1AAC3, 0x3448EC29, 0x358544C8, 0x372B2F97, 0x37B16530, 0x3854D686, 0x37F9A04F, 0x3864F298, 
0x37373D62, 0x36F75181, 0x37A7C044, 0x378EFD6A, 0x373B098B, 0x37994440, 0x374BD77A, 0x3733A11C, 
0x3634A05F, 0x3724A634, 0x37522988, 0x37D07594, 0x378E05E5, 0x37B705C8, 0x374FCE88, 0x3821AEA4, 
0x373D3593, 0x3803EB15, 0x37590B34, 0x34A8CB09, 0x336B5407, 0x32356E79, 0x3053CBAC, 0x2EE337CA, 
0x32614E9B, 0x34700CF5, 0x3738508A, 0x392144CC, 0x38CBB061, 0x39E68A7F, 0x3A6B7D55, 0x39AFC510, 
0x391E9BDD, 0x387CB526, 0x38CA18A5, 0x39668C06, 0x38C0E9AA, 0x384B6169, 0x38CF39BE, 0x396982E4, 
0x39299AB2, 0x3872FA33, 0x394A8694, 0x3980BBFB, 0x39E4F22E, 0x39350109, 0x383095C7, 0x3961A249, 
0x39893334, 0x38DB0EC2, 0x38B86099, 0x35DAE30D, 0x3447D9B0, 0x31C47D49, 0x2FB04A65, 0x2F7CE815, 
0x31DA8F58, 0x346092AB, 0x36DD07C1, 0x398A21E0, 0x3A3245F2, 0x3B5D9060, 0x3AEF3BB5, 0x3AB28038, 
0x3AEC2085, 0x39AD1291, 0x39572B7C, 0x3A3E8C82, 0x3907662A, 0x3A127ECB, 0x39752158, 0x3ADDAEDB, 
0x39DE4DF3, 0x3A8F5D6D, 0x39FC4A42, 0x3B819AD6, 0x3A01BA51, 0x3A53A442, 0x3AACE6FB, 0x3B8A18B9, 
0x3B30B18E, 0x39BF52B6, 0x38B80A65, 0x35F95CFB, 0x3471D3FC, 0x31422A16, 0x304DCED1, 0x2FC268DA, 
0x3285C6C6, 0x33859B64, 0x373C8A23, 0x394018AB, 0x39D492C9, 0x3CA2D6C9, 0x3B6961A3, 0x3B7E3276, 
0x3A9EC71A, 0x39F511EF, 0x39C0F583, 0x38CEB195, 0x3972492C, 0x3A51D9E2, 0x3A7A8827, 0x3A9756F0, 
0x3ADE9E97, 0x3AD6D80F, 0x3A16B250, 0x3996C445, 0x3AD13190, 0x392C26D0, 0x3A4A6C91, 0x3B62F85E, 
0x3B46614B, 0x3AD3EF1D, 0x38FFC3A5, 0x35D3A9CC, 0x340027A5, 0x3121A3C5, 0x2F74D821, 0x2EF0F093, 
0x329B9406, 0x33E3990E, 0x36AC6A2C, 0x38D97A94, 0x3B8FAB69, 0x3B9A4836, 0x3C2D0957, 0x3ACD937A, 
0x3A00F274, 0x39A6689D, 0x38C774F3, 0x38B56540, 0x39111C9E, 0x39180293, 0x39F96EB8, 0x3A6961A5, 
0x3AD499FD, 0x39A0F7CC, 0x39BC10A0, 0x3900A51A, 0x395DDB30, 0x38F02005, 0x39BF82B2, 0x3B72E627, 
0x3BBF8F1B, 0x3ACC9862, 0x39FB2830, 0x3781CD2F, 0x3439A63B, 0x31680BE3, 0x2F6F8B26, 0x2F889FB3, 
0x31D33F66, 0x343DCBE5, 0x35B5E4C6, 0x398A8BD1, 0x3B735F45, 0x3D11D031, 0x3C24A0FE, 0x3BBA192E, 
0x3A0FA3AA, 0x39208DC2, 0x375062E9, 0x37BF2201, 0x3760AEF8, 0x386928FF, 0x39E25315, 0x3B39A6D4, 
0x3B885521, 0x3AB02BA4, 0x3885C289, 0x39806726, 0x38942836, 0x3818399A, 0x393648CE, 0x3AFCB19A, 
0x3CD357BD, 0x3C02F56C, 0x3A48E17E, 0x37AB1826, 0x34FCE341, 0x3186235E, 0x2FEEF122, 0x2F971394, 
0x31F860ED, 0x34143216, 0x3704B200, 0x38E98F80, 0x3B853539, 0x3D1BB29F, 0x3D6AB0A6, 0x3BEB1DEE, 
0x396FED73, 0x378F338B, 0x365F698C, 0x3661BD30, 0x37B7DBC0, 0x38BA07B9, 0x38BBC4CC, 0x3AA131CF, 
0x3B294E33, 0x3A6E1A93, 0x3B0CBE81, 0x391E4ACA, 0x38F9E917, 0x389153B0, 0x3974847E, 0x3B901621, 
0x3CAF974E, 0x3C18EE0A, 0x3A8B72DC, 0x37E6C90D, 0x348B4115, 0x31527F0F, 0x2FEFA672, 0x2F21AE08, 
0x32083FC0, 0x3531ED42, 0x36F2C702, 0x3901F4D5, 0x3B650887, 0x3CF74CD9, 0x3D41EA9D, 0x3CBE5801, 
0x39E9A6FF, 0x39133C2C, 0x371898FD, 0x3719DDAD, 0x37DE5D2B, 0x3814F1DC, 0x38CD8916, 0x39E12901, 
0x3B524EED, 0x3BDDAA65, 0x3A0AA779, 0x39DA1DBE, 0x38883AA9, 0x389D8F37, 0x38B119CB, 0x39DF6930, 
0x3C3F6741, 0x3BC9A3AB, 0x3BDEDD1E, 0x3813F30F, 0x348B50EC, 0x320B8183, 0x2F78F0B0, 0x2EBFD746, 
0x318FCFE1, 0x35094C45, 0x3696043C, 0x38694A07, 0x3C0421B1, 0x3CE69F42, 0x3CC51ADC, 0x3BED09AB, 
0x3B5C4231, 0x387651BC, 0x3661ED53, 0x3666BA86, 0x372C0ECA, 0x37681DE2, 0x3925BFAE, 0x3A16568E, 
0x3B9708EA, 0x3A2C29C8, 0x39D53B7B, 0x38FDE60A, 0x3861EBC1, 0x3701D4E4, 0x37B0B193, 0x38A55908, 
0x3A94EA90, 0x3C04FF4B, 0x3A276714, 0x377419BF, 0x34369A03, 0x32E8535D, 0x30B29D1C, 0x2FDCC61F, 
0x31EDF88F, 0x34646033, 0x36C337A6, 0x393B26C7, 0x3AD9E58C, 0x3D45D032, 0x3CCA192D, 0x3B49BC6A, 
0x394FA2A9, 0x37ECE28D, 0x36951A60, 0x35FE8F0A, 0x3729557C, 0x38805FF9, 0x39ECB346, 0x3A338EEB, 
0x3A5CF2B9, 0x3A1AEDA6, 0x39F9CBA8, 0x38C5CC59, 0x3743EE0D, 0x36821144, 0x3635F63A, 0x37A44B37, 
0x39D4CF7F, 0x3BD60346, 0x39FF7EBB, 0x37F12A5C, 0x3537AFFF, 0x32DCD35C, 0x300210C9, 0x2FB7FE2D, 
0x331F5E63, 0x33FED925, 0x36CBB844, 0x392442FA, 0x3C324F49, 0x3D074528, 0x3CFF1CAF, 0x3C8A06B7, 
0x39D9A3EF, 0x373C20AE, 0x36623A88, 0x36469FF7, 0x3660D6EA, 0x37F5739B, 0x39695943, 0x3A188ED6, 
0x3B65B32A, 0x3A7BF68D, 0x3999DB73, 0x390C6192, 0x36E52FB4, 0x36A982C7, 0x3545DEF0, 0x378103B4, 
0x38685A5F, 0x3AB38914, 0x3AB48051, 0x37C7C22F, 0x35AD1888, 0x335E0EE9, 0x30CFF37C, 0x2F8B908A, 
0x330176BB, 0x33B55C02, 0x367007EA, 0x39DB2C36, 0x3C5FAD43, 0x3D5C0A15, 0x3DFF3D03, 0x3C76317B, 
0x3B2E0FC5, 0x383DCA1A, 0x369B9642, 0x3690BEEF, 0x37229ECC, 0x38072B7A, 0x3A28DF27, 0x3BF12B99, 
0x3C2BCD77, 0x3C0EF8E4, 0x3A5C5C56, 0x3884A3C8, 0x3769D66B, 0x362B7450, 0x353B017A, 0x37A3D007, 
0x3AF58C19, 0x3B412BF0, 0x3AD2F7CC, 0x386493C3, 0x367BA775, 0x32BA5CEC, 0x3078D2CA, 0x2F831EAB, 
0x328A1C21, 0x34E31C5C, 0x36A11519, 0x39855128, 0x3CFB35BB, 0x3F0ECE5A, 0x3DDE91C9, 0x3D1C0FDC, 
0x3B6EEDFF, 0x39197AFC, 0x379E2CEF, 0x37FD80FF, 0x38CEC631, 0x3A4281F8, 0x3D19D34C, 0x3DB2FAB6, 
0x3DA71240, 0x3CBE4EBA, 0x3BDC5F01, 0x3A31E86E, 0x38566A2D, 0x36781175, 0x36AFD2EF, 0x39734FBB, 
0x3B292034, 0x3C726CF0, 0x3BD30B00, 0x3919820D, 0x363A9B0E, 0x32ADAA45, 0x30ABFAD5, 0x2FB4355D, 
0x32C0FD27, 0x3527B90E, 0x36B41578, 0x3A5D10F9, 0x3DB32092, 0x3F1A8091, 0x3F88B936, 0x3E10D72B, 
0x3B91E893, 0x3AA44353, 0x39D8C3F1, 0x39464F74, 0x3ACF5F88, 0x3C26693B, 0x3D5A09EE, 0x3E93E3A2, 
0x3E92F3A1, 0x3EA8F7E4, 0x3C743102, 0x3BE6ED03, 0x38334A53, 0x3765A03F, 0x37F28293, 0x3B02D590, 
0x3CE738A9, 0x3E4E9CC1, 0x3BFDA2B5, 0x39D8ABB9, 0x3700F805, 0x330CAE4C, 0x30979075, 0x300FFFF3, 
0x321842E6, 0x34D944B6, 0x36CE9FC4, 0x3ADB98E4, 0x3C766FF1, 0x3D7BD2DB, 0x3E43731A, 0x3DDECD77, 
0x3C93246E, 0x3B29BD34, 0x39C3E31B, 0x3A6D9E75, 0x3B6B28DB, 0x3C47105E, 0x3D43C98B, 0x3DDBAEEA, 
0x3F45853F, 0x3EBD67E2, 0x3C8914FB, 0x3A0783F3, 0x3A29F2F2, 0x38B0E746, 0x394F79EE, 0x3B8A166B, 
0x3E49DAD6, 0x3E453CE5, 0x3D57CF0B, 0x397632D0, 0x371D5607, 0x33480152, 0x30B81892, 0x30006B7B, 
0x32C73E14, 0x3333BFEA, 0x36D0D32C, 0x3822CF92, 0x3A664060, 0x3C752E7D, 0x3C90B7B6, 0x3C29706F, 
0x3B3AFCE8, 0x3A111E14, 0x3A80DC98, 0x3A0A777C, 0x3AC21AB5, 0x3BC16E08, 0x3CAEA5BE, 0x3CF904A9, 
0x3DE8939E, 0x3C407201, 0x3B6C2B81, 0x3A45E8AB, 0x3928906D, 0x38A4C0F6, 0x39B0C26F, 0x3AB3EC82, 
0x3C90B469, 0x3CA45B54, 0x3C09079A, 0x3943245D, 0x35EB3721, 0x33C8230D, 0x313483BA, 0x302C24DC, 
0x316BD8F4, 0x32C2D3E8, 0x3413B3A8, 0x3697DC36, 0x382CBF55, 0x39C7AA94, 0x3A980737, 0x3A2507A0, 
0x3864D7BB, 0x380FC74C, 0x37833C70, 0x36D58C8E, 0x384E59CA, 0x394FFE2F, 0x39E7A0B5, 0x397890FD, 
0x3A71FD7D, 0x39D4D29E, 0x399AE481, 0x375C6D2C, 0x37D36A87, 0x361E9816, 0x36AC27D7, 0x37B8596D, 
0x3A43A8AD, 0x3A17C462, 0x394838FF, 0x371DF899, 0x34F85456, 0x32A941B4, 0x311D5921, 0x2FE57E86, 
0x30629011, 0x31F0C1E6, 0x334BC4DA, 0x34B72C56, 0x360EFDE4, 0x36A7D9ED, 0x38948C10, 0x387FD6F5, 
0x375EEFCD, 0x362D56C9, 0x3690B8B1, 0x36CE21D9, 0x37124ECE, 0x3749AD1E, 0x376572D9, 0x3896DADC, 
0x384E919F, 0x38EC06B4, 0x36D8A88E, 0x3667C21A, 0x35575019, 0x3581FE67, 0x3541775C, 0x36CFD02A, 
0x3760C96A, 0x37914A77, 0x36449C6A, 0x35B70500, 0x34B57D9F, 0x3229100D, 0x304E3265, 0x2F3E2654, 
0x307F5C31, 0x3070C03E, 0x3236C008, 0x32EC5DC4, 0x332C0C52, 0x360505D6, 0x3526CA29, 0x360E545B, 
0x3514D4F0, 0x34EE328C, 0x34C0CAB7, 0x357A1CFF, 0x350D931F, 0x34E931E3, 0x35BF7307, 0x35D75E9A, 
0x36877636, 0x3589D843, 0x36197361, 0x34B887CA, 0x34848055, 0x340F575F, 0x32E5CB34, 0x345DBAD2, 
0x351C8519, 0x34C665C1, 0x345D14A0, 0x3408402B, 0x32D6B824, 0x31FF674A, 0x2FE67D2A, 0x3065E575, 
0x304052AF, 0x2FF92AB5, 0x313E0304, 0x31AA348B, 0x32A2997E, 0x33391EF9, 0x348ADEF8, 0x340CAF54, 
0x349E31B0, 0x336D10FA, 0x334728E4, 0x338B1526, 0x33FD046A, 0x343499D4, 0x33CEA3E8, 0x341D08F2, 
0x33768165, 0x339C5DCF, 0x330BAFAA, 0x33427795, 0x338D7B4E, 0x32FD3650, 0x32731CF1, 0x3285CECD, 
0x3368DDC8, 0x330CA714, 0x333DFB73, 0x327DE82E, 0x31CFB946, 0x312893A0, 0x30098332, 0x2FEBEBA2, 
0x2FC79642, 0x301168E3, 0x300AD372, 0x31038009, 0x311994FC, 0x3178BBF6, 0x32955D33, 0x32BF194D, 
0x334F6DE5, 0x3181A0F3, 0x317E593E, 0x31F431ED, 0x31FA9E88, 0x31A03B4C, 0x31FF3123, 0x329D9747, 
0x320EE4DA, 0x327C997B, 0x31AE9D84, 0x3125A083, 0x3188A9A4, 0x318721F1, 0x312B8C29, 0x31B1C334, 
0x31BCE815, 0x317FC5C9, 0x31A6A3EF, 0x30B39F1A, 0x314B6975, 0x2FB5D0EA, 0x300A71F8, 0x2FB18DFC
},
{0x2FE2B45C, 0x3200E963, 0x32DB7753, 0x32F34C7B, 0x3306E274, 0x349CEA61, 0x336212C1, 0x32F86E87, 
0x328583E9, 0x336C410B, 0x344EBF5A, 0x3352E380, 0x3376FA3E, 0x3353339A, 0x33E79102, 0x32D6305B, 
0x32F3C71E, 0x32FE0F58, 0x3209ECFD, 0x317753A3, 0x3226483F, 0x3190E150, 0x325BD68D, 0x32BD2FBD, 
0x3446B663, 0x33948E18, 0x333A9A18, 0x31D715F0, 0x317377CE, 0x3037C229, 0x2ED3F199, 0x2EB207E8, 
0x3168B7EB, 0x33412E04, 0x34A75018, 0x35EAAC39, 0x348F9139, 0x36A0D485, 0x37363FB5, 0x378CBCF2, 
0x37AF3190, 0x38C6485F, 0x38885FD7, 0x3852AEBB, 0x377D23B3, 0x36DCB6E7, 0x36B4E0E1, 0x3634D8EF, 
0x35FB4242, 0x359680B5, 0x358E12BC, 0x34680712, 0x34E71F40, 0x36181573, 0x345CBF63, 0x35A7AC3D, 
0x3539106F, 0x35BC959D, 0x34AED634, 0x32BCACFC, 0x31C2B194, 0x3150E67C, 0x2E8B4890, 0x2FF2BBE0, 
0x31868537, 0x3350D78A, 0x36147D72, 0x36F3F508, 0x38FDCD3A, 0x3BC73722, 0x3C5C1F2A, 0x3EDA1673, 
0x3F80A70D, 0x402F586B, 0x3FD78B1F, 0x4025D9B5, 0x3DB866AD, 0x3C77B93C, 0x3C7B43B1, 0x3B42ED5B, 
0x385492D8, 0x397DF6BA, 0x37FDC5A5, 0x37BB1359, 0x37F17710, 0x3755B728, 0x37B9F2AF, 0x37984633, 
0x3775F7FA, 0x3698495F, 0x358B1CEC, 0x341FA5FB, 0x32610801, 0x30F711AB, 0x2FABC5FD, 0x2FE25D15, 
0x3243A234, 0x34D5D0E6, 0x37497F24, 0x3B5BE11D, 0x3E01A81C, 0x425665B9, 0x456435B1, 0x482784F6, 
0x49B98F6E, 0x4A8EA9E5, 0x4A59130D, 0x4AD944BD, 0x4A06C00E, 0x481CBFFF, 0x4748DAF2, 0x432296F2, 
0x4168811A, 0x3F6A5CD9, 0x3C705E81, 0x3B844924, 0x3B2F6590, 0x39BE8A7C, 0x39716F88, 0x38D8B0AD, 
0x3807F94D, 0x38B7065B, 0x36CB5436, 0x350705D8, 0x33057920, 0x31B3D21C, 0x308D9E77, 0x2E8ECEF0, 
0x32187A98, 0x3590D620, 0x386EB304, 0x3FB013D0, 0x44099C61, 0x48A94522, 0x4D23060D, 0x4EE92070, 
0x54CDCE9A, 0x559B3648, 0x555B34A8, 0x55D8ADCA, 0x57574687, 0x534D158E, 0x514916DE, 0x4C7CDB59, 
0x4929320C, 0x4675FBA3, 0x43481AD6, 0x413ACE57, 0x3D4AAAE9, 0x3CD0212D, 0x3C5192FA, 0x3C0B0EE9, 
0x3C900E6A, 0x3B52CB42, 0x3905528C, 0x3655E919, 0x341281A1, 0x318B6874, 0x3183E518, 0x2F0A0162, 
0x33156787, 0x37427A00, 0x3B86BAA9, 0x43637A06, 0x4A1089CB, 0x5135A8A0, 0x5722EE49, 0x5A809B0C, 
0x61EC8078, 0x6333BAC7, 0x63A50FD8, 0x645AF2BF, 0x627DA383, 0x5D9174D1, 0x5BA8B658, 0x54F8FE4B, 
0x5121A117, 0x4C725FAD, 0x4A18EA27, 0x44DC3192, 0x4330B2E4, 0x4174A544, 0x3FF44B8F, 0x402FAC32, 
0x3F26E6A0, 0x3ECDDA05, 0x3B387637, 0x37C4C25D, 0x34E988E9, 0x32A3CAF2, 0x3016D988, 0x305B1899, 
0x3426EC82, 0x36CADFB1, 0x3CA41013, 0x45C5D4EF, 0x4D62C033, 0x56828FBB, 0x5F4B5B9B, 0x667286F7, 
0x6AC2A981, 0x6F2BBE6D, 0x70B77531, 0x6F80E454, 0x6C778BC0, 0x685A027A, 0x63D8F886, 0x5CDCF98D, 
0x56C002C5, 0x51117349, 0x4C0036F6, 0x4977AC5A, 0x44F8E69C, 0x414BE51F, 0x40EBFAE1, 0x3F9629CA, 
0x40CDC2BB, 0x3ED971F9, 0x3C0066F9, 0x382FA8A5, 0x3586B14A, 0x31D590ED, 0x30CBABB0, 0x2EE8A592, 
0x33FE15C6, 0x3726D9EC, 0x3DC32356, 0x481FBF5D, 0x52D9CEE2, 0x5B96A9A5, 0x65D5D3BA, 0x6E3F3FA4, 
0x76A52866, 0x7B32D0E1, 0x7C9F43C2, 0x7C4655D4, 0x784C28C7, 0x72FFC555, 0x6D99F624, 0x64562DA9, 
0x5DE07F37, 0x576AECD1, 0x5139299E, 0x4B26C49A, 0x477E0032, 0x436200C8, 0x419572B3, 0x415E0473, 
0x4088F6E4, 0x3F13AE78, 0x3B3657DA, 0x38E8501A, 0x34FA8740, 0x32892585, 0x30422676, 0x2EE7AC44, 
0x33108906, 0x36B4BB18, 0x3EB8BFEB, 0x4A9978D6, 0x5532BE10, 0x5F1BA5C6, 0x6B297A11, 0x77FDBB0B, 
0x7E8239E8, 0x846B90CE, 0x88164B00, 0x85B4DA45, 0x83E0989E, 0x7C3F8784, 0x7540294C, 0x6CB7A6B8, 
0x6385FF85, 0x5A51A05C, 0x542376BF, 0x4F20A1AE, 0x497D34F2, 0x46099756, 0x42789DF5, 0x41237162, 
0x3F4F7F87, 0x3D8E8576, 0x3C3FE848, 0x37974C0C, 0x34A50EFF, 0x31E238E7, 0x2F65EDB2, 0x2FD56053, 
0x3277C257, 0x37736F39, 0x3F31205A, 0x4B020F2B, 0x56A33B21, 0x63FB1646, 0x717379BC, 0x7D7DC43E, 
0x87D49436, 0x8ED84330, 0x8FD80E33, 0x8F272462, 0x8BD83052, 0x85F5B4E7, 0x7B91E8B6, 0x6EC5E3F0, 
0x66756B9D, 0x5FA06455, 0x5755B609, 0x51D50BD9, 0x4AD9C643, 0x469A6481, 0x4248B068, 0x4071DEB6, 
0x3FEE50D1, 0x3EB687B9, 0x3C65287D, 0x37DFFB5F, 0x345A1AF3, 0x32314185, 0x30116DEE, 0x2EE5C98D, 
0x32633F7B, 0x378BC6A6, 0x3F8998C6, 0x4C63E987, 0x591BA0AB, 0x670E306C, 0x762B59D1, 0x863453D7, 
0x8FAE3AE7, 0x93D84B97, 0x9973AC38, 0x972725DA, 0x93A2FA28, 0x8BF944CE, 0x80AADBDC, 0x75108112, 
0x6A6B4FAE, 0x622502AD, 0x58456CDF, 0x5122CEE6, 0x49868DD4, 0x46F21B79, 0x417753C6, 0x3FFCAE3B, 
0x3E99D89A, 0x3D0673BE, 0x3CC49942, 0x386E45BA, 0x352A6A59, 0x312723F9, 0x2FF4AF5D, 0x2F524CDC, 
0x33F387C9, 0x37920E87, 0x3F8550B0, 0x4D2F2ECA, 0x5A85CCD7, 0x6A69B9EE, 0x7BA3EF8C, 0x8BD7E1DA, 
0x954F7ADB, 0x9D119185, 0xA0C6324D, 0xA0CE2433, 0x9ADFB2D5, 0x92E82438, 0x87D7E371, 0x79B014EF, 
0x6EF15217, 0x634EA7D5, 0x5AA4ED77, 0x4FCB16D2, 0x4A60AF4C, 0x45036A46, 0x418B856E, 0x40C055C5, 
0x3FF8500D, 0x3D757887, 0x3ECEA7ED, 0x3AA22A67, 0x35F3F937, 0x32EB0284, 0x2FF05BFD, 0x2EF3995B, 
0x32F21C8E, 0x3955F827, 0x41AB2262, 0x4F15D293, 0x5C6805AB, 0x6CB77598, 0x7E58115B, 0x8F4CE82B, 
0x9C223DEC, 0xA56E9506, 0xA824A8AC, 0xA7DCA616, 0xA2397A37, 0x9823C4FC, 0x8C11C051, 0x7E256390, 
0x712F5A49, 0x665A34BF, 0x592B72CD, 0x4F91867C, 0x47EBF6FF, 0x44C9CA88, 0x40A72403, 0x4080CACF, 
0x4015C028, 0x3FF25078, 0x3E099C85, 0x3A9AEAF5, 0x35FB73A9, 0x32F0AEB1, 0x31910293, 0x2F456727, 
0x33C4C412, 0x38C2BD40, 0x42BF5A9D, 0x4EFDDD24, 0x5CA8FC40, 0x6D964DDF, 0x80759475, 0x91EF4A9C, 
0xA0793631, 0xAA1A60C8, 0xACF79378, 0xAD54205A, 0xA82222A2, 0x9B8F41EA, 0x8DF37D85, 0x81200FA8, 
0x7385475F, 0x6593DFBC, 0x59C37397, 0x4F511B13, 0x477255DE, 0x41C33F0A, 0x40E37A31, 0x3F63838B, 
0x40C55C3B, 0x414F5ABF, 0x3F7D8E23, 0x3A79EB06, 0x3730540F, 0x32ED7295, 0x317771EF, 0x2FDD303B, 
0x334886FA, 0x39995C63, 0x4303C5FB, 0x4F86FB4D, 0x5DD40B3F, 0x7130362D, 0x8174EF56, 0x92339690, 
0xA14B6978, 0xABFBB260, 0xB052E885, 0xAEBE6BDF, 0xA6B99F24, 0x9E9424C3, 0x8F5AEC82, 0x80EBB1D1, 
0x7410C532, 0x63CD6BF1, 0x581DE8AD, 0x508F9083, 0x483B8440, 0x40EF5715, 0x3FE5F371, 0x40D82BC1, 
0x41B5A431, 0x409E3AC8, 0x4004C236, 0x3B83EC2A, 0x37167B33, 0x3390B966, 0x316D606C, 0x2F40C90D, 
0x34B233A0, 0x385E7CB0, 0x4388557F, 0x4F270BFD, 0x5E307EA0, 0x6EFEBBFD, 0x80F08A8D, 0x90199385, 
0x9C887BC0, 0xA7A5E735, 0xAC696AC3, 0xAB4CB180, 0xA69553C5, 0x9C021825, 0x8C3219AE, 0x7E579E47, 
0x718CF949, 0x643CAD96, 0x59C7817D, 0x50B52056, 0x47DA9A0B, 0x425F480D, 0x4134BD1C, 0x40BA132A, 
0x41506909, 0x41CEB6F9, 0x40E125FB, 0x3A9B62C1, 0x37BFA66B, 0x33E2C9AB, 0x2F3AA5E4, 0x2E6A659C, 
0x34585B2A, 0x399008BC, 0x43FDF90D, 0x4F9009F5, 0x5DE9DCD3, 0x6C266866, 0x7A6B6010, 0x8B11D635, 
0x9650A7A9, 0xA04E1814, 0xA5360967, 0xA4A349EC, 0x9F76896F, 0x95CBA54A, 0x88E00ACC, 0x7B98FF22, 
0x6F04EB4D, 0x63A4B41C, 0x58C184C5, 0x50014C44, 0x463C7BD9, 0x419917AE, 0x41B48747, 0x41E871F5, 
0x415AF650, 0x41AA788A, 0x3EEE162B, 0x3AC78C32, 0x37A86FE8, 0x338E3310, 0x310038A9, 0x2F0AE393, 
0x36511AB4, 0x39EE787A, 0x428689F2, 0x4EAD927E, 0x5A625856, 0x6729C158, 0x7471EC7D, 0x828DD445, 
0x8E237FC4, 0x97073B1F, 0x9A5D0FDA, 0x9B731EF1, 0x94225386, 0x8DEE50A1, 0x80AA11FF, 0x788EDB5D, 
0x6D5ECD31, 0x620B0DE6, 0x580E8850, 0x4F20AE0D, 0x464B4B03, 0x42CF7AD6, 0x40C559DC, 0x3F79F900, 
0x4080C0BC, 0x40D26A2B, 0x3F0127C3, 0x3A6767BB, 0x370DD984, 0x34CD6D7B, 0x2EEE2CFC, 0x301E760A, 
0x35B1DE26, 0x3B11CC34, 0x4149813F, 0x4AF4F7CE, 0x556C5292, 0x60DEB953, 0x6D19D354, 0x79CDF131, 
0x8233D462, 0x89C504AC, 0x8DDA140C, 0x8BF63297, 0x8795C843, 0x820DF474, 0x786AEE3F, 0x6EA85D51, 
0x66EF1355, 0x5CB64642, 0x53D90E74, 0x4D5E030D, 0x46B1D7E2, 0x4283D7B4, 0x3F36CB60, 0x3DE79F9E, 
0x3E9E619F, 0x3D12B717, 0x3BAB26E8, 0x39E2125B, 0x37664786, 0x32A838B6, 0x308E6345, 0x30050413, 
0x350A4A72, 0x395B34B8, 0x3F0900FA, 0x4775BF66, 0x4FB112BD, 0x599B17CC, 0x62A14A09, 0x6B3638ED, 
0x738A7710, 0x78BC3346, 0x7C6C4082, 0x7B8B7F5F, 0x7972263D, 0x72C684D4, 0x6AD9E4D4, 0x65939B4B, 
0x5D2E253F, 0x54A76FF0, 0x4DF5D510, 0x494B53BA, 0x442EC160, 0x3FA0A9F4, 0x3C4E814A, 0x3C95726F, 
0x3B63A49D, 0x3B3A8ADC, 0x3A647666, 0x370C2343, 0x34BE9906, 0x31DB2D75, 0x31E357AE, 0x30265496, 
0x34AC74D8, 0x38D2CFEC, 0x3D897170, 0x4344B6C1, 0x4B1A2815, 0x51A593CB, 0x5AAD36AE, 0x62A7D05B, 
0x66C2DC1D, 0x6B69F5CC, 0x6D06779D, 0x6D8A0495, 0x6BE64A3E, 0x68BB27E8, 0x61F520E1, 0x5CA0CC80, 
0x56CDAF54, 0x4F3EB4B3, 0x494D2BDE, 0x45BBB955, 0x3F996F53, 0x3EA213EE, 0x3C05702D, 0x39E0D680, 
0x3A275394, 0x376EC888, 0x37983216, 0x36B10C84, 0x331367D3, 0x313CED48, 0x305EAB93, 0x2EF4C110, 
0x33C9F0D2, 0x37F17695, 0x3AEDC6AF, 0x414BBE71, 0x4796823B, 0x4D50CA40, 0x533FC41B, 0x580084CA, 
0x5B859DB5, 0x5E43013A, 0x5F8F2378, 0x5F78C123, 0x5D2069A3, 0x5C226916, 0x587FE0D9, 0x551F8171, 
0x4F22F472, 0x4A9CC157, 0x45D19D4E, 0x41BE0054, 0x3E5DEB89, 0x3C36A257, 0x3A7B30E0, 0x3838ADC7, 
0x371C83C7, 0x36694D19, 0x357E159D, 0x33EB1C4F, 0x3303C2F4, 0x3052B0AF, 0x2F2B2771, 0x2F694375, 
0x341D720A, 0x35CB4690, 0x38F08827, 0x3EAB9EC6, 0x421317D2, 0x46294F3C, 0x4AF176ED, 0x4E865435, 
0x517A20CE, 0x52C337C5, 0x53627C43, 0x53DBC910, 0x54ED81EE, 0x51536EAC, 0x503F1AD6, 0x4DF52858, 
0x4A5548DE, 0x45E3E308, 0x40BA293B, 0x3D008741, 0x3C38C47E, 0x3894BA74, 0x384673F2, 0x36295512, 
0x35867787, 0x3524FA11, 0x34A29A09, 0x336BACC5, 0x30E40E60, 0x301ED319, 0x303EF915, 0x2EC2B03F, 
0x33DE3EB9, 0x350BCF86, 0x37843883, 0x3A30CDE3, 0x3CF837B7, 0x410914EA, 0x456D3A6B, 0x46EE7309, 
0x479B4FAC, 0x48B562D6, 0x49ABA6A9, 0x4AE8C1A8, 0x495C0D3B, 0x494BE718, 0x46AE21F5, 0x47279BE4, 
0x44D91E07, 0x40DC4134, 0x3C111F7A, 0x3928C4DE, 0x39624586, 0x35E6958C, 0x35B04700, 0x333B30DE, 
0x331CAE1A, 0x315B8BDF, 0x321814A4, 0x305F6F4E, 0x2F895DEC, 0x30A088C2, 0x2F6A753D, 0x2FD0198E
},
{0x319C97CE, 0x315DF4F3, 0x3153337D, 0x321991AE, 0x32F2D3F6, 0x32C04C80, 0x33ECCBBA, 0x332C32D0, 
0x343776AB, 0x3289230A, 0x31F8AE55, 0x30FD2F26, 0x322586D1, 0x30AF6BE8, 0x32DEB60E, 0x3343167F, 
0x3349F8D2, 0x32D16598, 0x3268676D, 0x342F9A62, 0x331B987D, 0x33418100, 0x33F8EA44, 0x33FC757D, 
0x339D878C, 0x3310B54C, 0x3361AF1B, 0x31AB31A2, 0x30F2D778, 0x306B808F, 0x2F814D55, 0x2F5370CC, 
0x31A97B50, 0x3260682F, 0x3317B720, 0x344EAE36, 0x34A5830C, 0x34C3615E, 0x34F18C81, 0x35302405, 
0x351B52C8, 0x34AD2C2C, 0x33FA72F5, 0x338BC1B7, 0x32F7A16F, 0x34344D19, 0x35EB072D, 0x34FB8754, 
0x360D4446, 0x36F13C93, 0x36BFD2AB, 0x377DF1C1, 0x369C12A0, 0x385B2830, 0x39093CAF, 0x39004EE0, 
0x39475AA1, 0x37A3BD7F, 0x36FE2D2E, 0x35203BB7, 0x332E54F4, 0x314AF579, 0x2E6EC8B0, 0x2EF5A7FF, 
0x31BEB126, 0x32CF6CE4, 0x3330009C, 0x34E5CA5A, 0x3762F0A9, 0x372DB703, 0x3843940C, 0x3663CD77, 
0x378BCB00, 0x37D836DD, 0x378D3285, 0x366DDE61, 0x379543F5, 0x37D9072B, 0x38D5292B, 0x39D6B4F8, 
0x3B32A026, 0x3C0FB08F, 0x3D0F93A8, 0x3E58185F, 0x3F2331CC, 0x3F8CB874, 0x412688B6, 0x3FBFC0EB, 
0x3EEF22B4, 0x3E58FBFE, 0x3B164E96, 0x36F19E2C, 0x346A4963, 0x32CCE5C6, 0x307A95FB, 0x2F9D51B9, 
0x30AF4101, 0x33FDE05A, 0x3493E722, 0x360ED59A, 0x381250EC, 0x3A866B29, 0x3A364D0E, 0x3AB9C8E6, 
0x3A52FE8B, 0x3A2DB135, 0x394E337A, 0x3AA7D5A5, 0x3A9A5DFB, 0x3BC99272, 0x3DA4CDF0, 0x3F201B04, 
0x4314A9F4, 0x432EB0D4, 0x457BAF7A, 0x482B6866, 0x49776F33, 0x4ACF7BC4, 0x4AF7D092, 0x4A8B59AA, 
0x46D0432F, 0x46BDAC62, 0x4174C195, 0x3B69023B, 0x36D3724E, 0x32921864, 0x3042E543, 0x2EFB88E3, 
0x320C6D6F, 0x346BDC0F, 0x3571D386, 0x37F04360, 0x3A1872B0, 0x3BE5D049, 0x3D42EAD3, 0x3D8ECE45, 
0x3C13652F, 0x3DF82697, 0x3CADD851, 0x3D636230, 0x3F267866, 0x41A5ADAE, 0x424B4506, 0x46BB115C, 
0x49CC33EB, 0x4C28F0E2, 0x5151C3FA, 0x52C2A98E, 0x54A21FE2, 0x549D6484, 0x56C7DFA7, 0x5550D399, 
0x51C2BEB8, 0x4EACB951, 0x487AB46F, 0x409071C1, 0x3A58B80F, 0x3531F7AB, 0x3080B559, 0x2FBE6F7C, 
0x32182A41, 0x3304ED05, 0x36830040, 0x392DB571, 0x399522A2, 0x3D8CD732, 0x400FC99E, 0x41B4F8F7, 
0x3F2D2FC3, 0x407CE18F, 0x3FAAC516, 0x41AB6A18, 0x42B7DD9C, 0x44E66E42, 0x499734E6, 0x4CC87B27, 
0x516244A5, 0x55DE2BB0, 0x5BD2D673, 0x5E6EE260, 0x62995F55, 0x62918BA6, 0x62517144, 0x62721B79, 
0x5EE9A256, 0x57D4DC85, 0x4FF63AFC, 0x4532B3DB, 0x3ECBC623, 0x36A7F851, 0x32027EE8, 0x30B79A74, 
0x327B86DD, 0x33F94B9F, 0x36FB368A, 0x394A123F, 0x3BE9B85E, 0x3F6F4246, 0x3EB9307A, 0x40ED114D, 
0x412D0CC9, 0x4209093D, 0x42CADC38, 0x4384FA30, 0x44F7632E, 0x49EF0F91, 0x4D466E28, 0x50FFB5DA, 
0x5852F9FA, 0x5DF4757E, 0x6325ADA0, 0x680498A8, 0x6AFC6C14, 0x6BA63ED1, 0x6DAFEF48, 0x6C3BA0E4, 
0x695183E4, 0x60753CF7, 0x570DEA45, 0x4C444094, 0x41986AF4, 0x38AADD98, 0x319DB36B, 0x2F449EBC, 
0x32F8C2A1, 0x33B4E182, 0x36EA44C5, 0x3A11A5CA, 0x3D89DCAC, 0x3F5D3707, 0x3FD0D1F2, 0x42573610, 
0x42C0D88F, 0x42460144, 0x4268299F, 0x4405B02B, 0x48819BD2, 0x48EE3B08, 0x4EAB3D48, 0x56334986, 
0x5D522B4C, 0x631D8FBF, 0x6AEDB16D, 0x707362AF, 0x726607C1, 0x77BDB7B6, 0x7A4ED99E, 0x76B52C0C, 
0x6FDE912C, 0x6B5A800F, 0x5E0D033F, 0x5074AE3B, 0x444B165D, 0x384C8247, 0x32EBE3B8, 0x2F301756, 
0x3190FAF4, 0x341A9783, 0x36F031DA, 0x3AF66E2C, 0x3EB0378B, 0x417BC1D7, 0x42A6B554, 0x43CB78C1, 
0x440D2F44, 0x42CC15FF, 0x43A7C4FF, 0x46111004, 0x4AD2D683, 0x4C8039D0, 0x51CCD112, 0x59F92DA3, 
0x6181B7F8, 0x6ABF21B3, 0x74650419, 0x791D3978, 0x7D3BBE0D, 0x8106CF0B, 0x855820B9, 0x81033716, 
0x7B1037C6, 0x6FCD559B, 0x642CE7B9, 0x540BB0CB, 0x47CEEBFA, 0x3BB985F8, 0x32727305, 0x2FC53FFE, 
0x3316222D, 0x3453F049, 0x3886BD4F, 0x3B40A97F, 0x3EB0BB2F, 0x426624C7, 0x4273D28C, 0x44271E8C, 
0x4404D8FD, 0x418BF853, 0x432FCB3A, 0x46DFED46, 0x48F01577, 0x4DC04207, 0x55130649, 0x5CDF5B1E, 
0x677289C2, 0x70CCB156, 0x793869E1, 0x819EC59B, 0x89B2A047, 0x8E7E5BF7, 0x8FC22B74, 0x8C800E94, 
0x835ADF74, 0x77C69E69, 0x6A80F39D, 0x58EDEFD5, 0x48DD092B, 0x3BF668A4, 0x332C5583, 0x2F729F5F, 
0x3263DFCB, 0x34A64FDF, 0x39602B85, 0x3C8911F2, 0x3F86D147, 0x42A546C3, 0x43E4F1E0, 0x442DD012, 
0x449631FC, 0x4307A55E, 0x444A521F, 0x43DC3FC7, 0x4B7B3A05, 0x4F7B6BD8, 0x56D71B50, 0x5F4DA134, 
0x6C90CBC2, 0x74C1FFD5, 0x7FE5A257, 0x8A52651D, 0x92E401A3, 0x9727840D, 0x9A945FFA, 0x9490015D, 
0x8BE01BC0, 0x7FB4D237, 0x6F9BC4E5, 0x5C946465, 0x4C0A8CBE, 0x3DB3B3B4, 0x33DE7C49, 0x306586F2, 
0x32A46C36, 0x344DD8C4, 0x38176CF2, 0x3C7B3608, 0x3F1F183D, 0x431DC38D, 0x43845F2A, 0x457B5DD8, 
0x44D3DE9B, 0x44594721, 0x42ECFAC0, 0x44D7AF34, 0x495EB29C, 0x51C6F534, 0x59ED3969, 0x651B5960, 
0x6FEE2FBD, 0x7A0ACA5C, 0x84B29FE3, 0x907C8E66, 0x9C518AA3, 0xA14AC257, 0xA17C900E, 0x9EE3C999, 
0x93340D8E, 0x85BDC7C6, 0x742106D9, 0x5E56667C, 0x4C40EAF5, 0x3CD7A6A2, 0x335072B9, 0x2F523F1B, 
0x32CA6D64, 0x350CD518, 0x38712294, 0x3D7755E9, 0x3F97B178, 0x427DE0B1, 0x429CE38D, 0x44AF6136, 
0x4488B17E, 0x4324EE39, 0x423205C8, 0x46152D2A, 0x4A976636, 0x5274BDE4, 0x5AAF1937, 0x65620F45, 
0x71E523EE, 0x7F65C6F4, 0x8C079ACB, 0x95DE6F40, 0xA32A833B, 0xAABC1AE6, 0xAE428266, 0xA5AFDBF5, 
0x9A7E6634, 0x8BEE2375, 0x7898D78C, 0x62934A3B, 0x4EE23535, 0x3ECA666C, 0x34748274, 0x2FAB9882, 
0x3357D780, 0x358BEF0D, 0x38CCBBD6, 0x3B2C938C, 0x3F952820, 0x41E9D740, 0x44C3E256, 0x4603E5BF, 
0x44C37ABF, 0x4326702A, 0x445739CF, 0x44A2442E, 0x4B6491E5, 0x53D76F6C, 0x5AC76DFA, 0x65D6B78B, 
0x718DA943, 0x80D166EE, 0x8E5D74F6, 0x9C4E1518, 0xA7D1D05D, 0xB110E31C, 0xB24DCECF, 0xAD2F5217, 
0x9F5D78E3, 0x8C66EF67, 0x7A18846E, 0x642D5246, 0x5034D22D, 0x3FEC1C0A, 0x3602676A, 0x306B5547, 
0x32B2F884, 0x36A15050, 0x37C512B4, 0x3CDC0913, 0x410D748A, 0x42E2E274, 0x44D46133, 0x46CE925D, 
0x43E60357, 0x43C2F608, 0x43FF48F1, 0x460D0031, 0x4B8702CE, 0x53554BD6, 0x5B3F5C74, 0x66A74B5D, 
0x7212DFB3, 0x7F4D7F57, 0x8DE1E596, 0x9C46702E, 0xAA8CA851, 0xB3BA273B, 0xB37B71D2, 0xAC89C097, 
0xA077AF63, 0x8E116191, 0x7CA5BCB8, 0x66263B48, 0x52A5C1B5, 0x425232C3, 0x36026D86, 0x3081AD4F, 
0x330D209F, 0x35C99E9D, 0x381D4844, 0x3D84D19D, 0x401D100F, 0x43CBDF06, 0x44260E9E, 0x45559125, 
0x460EDF8E, 0x43E7DA5E, 0x4521AF34, 0x474F5854, 0x4BABD4F1, 0x536F6A4B, 0x5AC78AC0, 0x63F3D8F9, 
0x7243698D, 0x7E049980, 0x8A853D3D, 0x9B25B8AF, 0xA589FCFF, 0xAE7CDE6E, 0xADDBC35F, 0xA94EFB23, 
0x9E013900, 0x8DB74757, 0x78A6A9B8, 0x655FFE4C, 0x51C2FED8, 0x4063CF6E, 0x381FC238, 0x317A6440, 
0x32721583, 0x3572A6EE, 0x391B1443, 0x3DA8B163, 0x4019DD25, 0x443FD296, 0x45808BCD, 0x452FE5A5, 
0x43DAD45B, 0x437D07E6, 0x449EAEEC, 0x46A65E78, 0x4BC74F1C, 0x540BCD63, 0x5B037055, 0x63D36A15, 
0x6F8ABA4E, 0x79AEA52E, 0x882280D4, 0x93CB9DAD, 0xA0685EE9, 0xA716F3AA, 0xA6379221, 0xA0AF1581, 
0x98E113A2, 0x8820649B, 0x76775F21, 0x635B52E4, 0x4FBAC976, 0x42797ABB, 0x37556CED, 0x317CA9E4, 
0x32A3B551, 0x36303AF0, 0x37C70289, 0x3C45CBB2, 0x406F0074, 0x41F4F499, 0x44A44C7E, 0x45722460, 
0x42D99397, 0x44C7DF27, 0x44798A5B, 0x467E622D, 0x4BB96FDB, 0x50BED83B, 0x57E9D592, 0x61CD7F13, 
0x6A9819CF, 0x756663D8, 0x8037124E, 0x8A0B6524, 0x95F83880, 0x99218014, 0x9A97326D, 0x98A953A5, 
0x8E76E6E4, 0x81CB8519, 0x7022EAD6, 0x5FF1C615, 0x4F617EA7, 0x418FDC77, 0x38203FBB, 0x3304E1DE, 
0x33C46F54, 0x351B4C26, 0x374BFD80, 0x38F4221D, 0x3E80D24B, 0x40CF02F0, 0x4143EB98, 0x422544FF, 
0x4242CE60, 0x418566E0, 0x414AF9FC, 0x448EFE80, 0x4896D9AD, 0x4F77C8A3, 0x55BFCB30, 0x5D3AF64A, 
0x6533A9E6, 0x6DAF867D, 0x789B6C82, 0x7FABD646, 0x8713FC38, 0x8D359E5E, 0x8D70AC36, 0x8A717F95, 
0x83B24583, 0x76F8AF14, 0x67AEDF70, 0x58E1A8EA, 0x4C6897EE, 0x40D4E60E, 0x38D7E755, 0x33849A07, 
0x31C2031E, 0x33AAE073, 0x359FB201, 0x38A7A266, 0x3BB706F2, 0x3D719648, 0x3E25CAFE, 0x3F1C53BA, 
0x3E7709B6, 0x3EB8DFD1, 0x3F411771, 0x41386FB9, 0x4401BC5F, 0x4A563CB9, 0x4FF0E880, 0x576DD54B, 
0x5F1D9831, 0x648FB54F, 0x6C52B4F4, 0x71BF6CA1, 0x7A3BA286, 0x7E48BD95, 0x7E4D7AFD, 0x7B5F5680, 
0x74FB4AA4, 0x6B2D90BB, 0x5EDF710E, 0x5287214C, 0x47B63807, 0x3E4381D6, 0x38702F74, 0x3487262A, 
0x31F6ABFE, 0x334677F9, 0x345BDC34, 0x3734901C, 0x383CD91C, 0x3ABD9ED7, 0x3B58BD2B, 0x3BD065B4, 
0x3BA5A536, 0x3CCD7575, 0x3CF2E585, 0x3E60A5B8, 0x41EC92DC, 0x470D7D6E, 0x4BDCD333, 0x51BA1874, 
0x57185CB5, 0x5AE190CC, 0x6023688F, 0x65DF7FCD, 0x6BD6B122, 0x71099285, 0x703E1EF3, 0x709C0C2F, 
0x69E3B091, 0x60E190A1, 0x57231BAE, 0x4EDEA15B, 0x43F77FED, 0x3C2442B6, 0x37489E75, 0x339EFE8C, 
0x31A6DFAA, 0x31DF9BB5, 0x32658D25, 0x3576F688, 0x35BE6776, 0x371C5C24, 0x37C878E7, 0x39E41E89, 
0x397EEE05, 0x39AD948A, 0x3A4EC494, 0x3C239BB5, 0x3EAE1C74, 0x43A64816, 0x46240DBD, 0x4BC8ED43, 
0x4DA58454, 0x53E809DB, 0x568076E2, 0x5C27A818, 0x5FE68003, 0x62B65F3E, 0x653F234C, 0x63CDFBD2, 
0x5DAC5105, 0x5857652A, 0x518C7A18, 0x48D4C81D, 0x41CEBD7B, 0x3C5FAD44, 0x380DB601, 0x349DB13D, 
0x30A4E221, 0x30A6B9E0, 0x318AB090, 0x323135C4, 0x34832C5D, 0x34EB5B5D, 0x36000D4F, 0x35F3FAFC, 
0x36BBF083, 0x390EC356, 0x396C9D2E, 0x3995BEE7, 0x3AEABD90, 0x3F065A0E, 0x42166D63, 0x445A002A, 
0x4761C604, 0x4B3563FD, 0x50258919, 0x530572A5, 0x56396435, 0x574D9FF1, 0x576C0700, 0x5868A9E7, 
0x54C4FE93, 0x4F24E5E6, 0x4B56A99E, 0x43657927, 0x3EAA618F, 0x3A89D889, 0x362B2D6D, 0x33C746CD, 
0x3050C260, 0x2F37638F, 0x3157408B, 0x30EF789C, 0x318B6B41, 0x33E611C4, 0x32853FF7, 0x337537AB, 
0x357D6217, 0x352B7E9E, 0x35B44BEA, 0x3762EEEB, 0x39A6FBEF, 0x3AF44F57, 0x3C7BA6CC, 0x3F88E3FB, 
0x4093695C, 0x4467C640, 0x46FD4613, 0x482B27CD, 0x4BA351F2, 0x4AF73FE9, 0x4BF4C1E5, 0x4BD5226C, 
0x4B653E15, 0x4954BD2B, 0x42F82519, 0x3E8677DE, 0x3A7E4499, 0x37BD0C9B, 0x33DB1DE4, 0x333EC013
},
};

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create objects for the IR sensor
Adafruit_MLX90640 mlx;
bool mlx_detected = false;

// Create objects for the sht35 sensor
Adafruit_SHT31 sht35 = Adafruit_SHT31();

// Network stuff
const char* ssid = "Mordor";
const char* password = "0ned0esn0tsimplyl0gint0m0rd0r";
const char* filenameUrl = "http://192.168.69.9:6969/filename";
const char* uploadUrl = "http://192.168.69.9:6969/upload";

// Create a client
WiFiClient client;
HTTPClient http;

// Ultrasonic time tracking
unsigned long ultrasonic_start_time = 0;
unsigned long ultrasonic_end_time = 0;

// IR buffers
float ir_data[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];
uint8_t mlx_buffer[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];

void setup() {
    Serial.begin(9600);

    system_update_cpu_freq(160);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    pinMode(LED_BUILTIN, OUTPUT);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display_splash_screen("Initializing...", caution_splashscreen, &display);

    pwm_init();
    ultrasonic_init();

    pinMode(TEG_PIN, OUTPUT);
    pinMode(TEG_AUX_PIN, OUTPUT);
    digitalWrite(TEG_PIN, LOW);
    digitalWrite(TEG_AUX_PIN, LOW);

    ultrasonic_start_time = millis();
    ultrasonic_start();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        display_splash_screen("Connecting to WiFi...", caution_splashscreen, &display);
        delay(500);
    }

    mlx.begin();
    mlx.setRefreshRate(MLX90640_16_HZ);
    mlx.setMode(MLX90640_INTERLEAVED);

    sht35.begin(0x44);
}

void loop(void) {
    // Get the IR data
    mlx.getFrame(ir_data);

    // Convert the IR data to a buffer
    for (int i = 0; i < MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y; i++) {
        mlx_buffer[i] = ir_data[i];
    }

    // Get temps
    int32_t water_temp = (int32_t)(sht35.readTemperature() * 100);
    int32_t hot_temp = calculate_max_temp(mlx_buffer);
    int32_t cold_temp = calculate_min_temp(mlx_buffer);
    int32_t temp_diff = hot_temp - cold_temp;

    // If no temperature gradient, assume no sensor
    // Use water temp
    if (temp_diff == 0) {
        mlx_detected = false;
        hot_temp = water_temp;
        cold_temp = TEMP_IDLE;
        temp_diff = hot_temp - cold_temp;
    }

    // If the sensor is detected, upload the data
    if (mlx_detected) {
        uploadThermalData(ir_data);
    }

    // Adjust TEG power, pump speed, and fan speed based on temperature readings
    adjust_teg_power(hot_temp, cold_temp, temp_diff);
    adjust_aux_teg_power(hot_temp, cold_temp, temp_diff);
    adjust_pump_speed(temp_diff);
    adjust_fan_speed(hot_temp);

    // Perform kmeans clustering
    bool cat_detected;
    if (mlx_detected) {
        cat_detected = kmeans_cluster(mlx_buffer, centroids) > 0;
    } else {
        cat_detected = false;
    }

    // Update the display
    String message = "Hot: " + String(hot_temp) + " Cold: " + String(cold_temp);
    if (cat_detected) {
        display_splash_screen(message, cat_splashscreen, &display);
    } else if (analogRead(PWM_ULTRASONIC_PIN) > 0) {
        display_splash_screen(message, ultrasonic_splashscreen, &display);
    } else {
        display_splash_screen(message, snowflake_splashscreen, &display);
    }

    // Check ultrasonic cleaning status
    check_ultrasonic_cleaning(cat_detected, &ultrasonic_start_time, &ultrasonic_end_time);
}

// Function to send MLX90640 data array
bool uploadThermalData(const float data[MLX90640_RESOLUTION_Y * MLX90640_RESOLUTION_X]) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return false;
    }

    // Get filename
    String filename;
    http.begin(client, filenameUrl);
    DEBUG_PRINTLN("Getting filename...");
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        filename = http.getString();
        DEBUG_PRINTLN(filename);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        return false;
    }
    http.end();

    String base_url = uploadUrl;
    base_url += "?filename=";
    base_url += filename;
    base_url += "&data=";
    size_t base_url_size = base_url.length();

    // get URL buffer
    size_t byte_size, base64_size;
    byte_size = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y * sizeof(float);
    base64_size = 4 * ((byte_size + 2) / 3);
    if (base64_size % 4) {
        base64_size += 4 - (base64_size % 4);
    }
    unsigned char *getUrl = (unsigned char *)malloc(base_url_size + base64_size + 1);
    
    // Zero out the buffer
    memset(getUrl, 0, base_url_size + base64_size + 1);

    // Copy the base url
    memcpy(getUrl, base_url.c_str(), base_url_size);

    // Encode the data
    encode_base64((unsigned char*)data, byte_size, getUrl + base_url_size);

    // Send the data
    http.begin(client, (const char*)getUrl);
    DEBUG_PRINTLN("Uploading thermal data...");
    httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        // Deallocate
        free(getUrl);
        return false;
    }

    DEBUG_PRINTLN("Thermal data upload complete.");
    // Deallocate
    free(getUrl);
    return true;
}

