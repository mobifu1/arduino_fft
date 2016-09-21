//##################################################################################################################
//##################################################################################################################
//##                                      FFT N=256  Integer                                                      ##
//##                                    [K]=X[N] \ E^2PI*K*n/N                                                    ##
//##                                f/s = 5 Khz   Niquist = 2.5 Khz                                               ##
//##                          1x FFT-Calculation = 52ms  > ca. 20/second                                          ##
//##################################################################################################################
//##################################################################################################################
#include <avr/pgmspace.h>
#include <TimerOne.h>
#include <Adafruit_SSD1306.h> //128x64 Oled Display


#define microseconds_1 200  //Timer1 in mikro seconds  default =100 =8,32 khz / 200=4.995 khz
//-----------------------------------------------------------------------------------------
int const re_wn[128] PROGMEM = {                                                     //reelle Wn Values  (Cos)
  256  , 256  , 256  , 255  , 255  , 254  , 253  , 252  , 251  , 250  , 248  , 247  , 245  , 243  , 241  , 239 ,
  237  , 234  , 231  , 229  , 226  , 223  , 220  , 216  , 213  , 209  , 206  , 202  , 198  , 194  , 190  , 185 ,
  181  , 177  , 172  , 167  , 162  , 157  , 152  , 147  , 142  , 137  , 132  , 126  , 121  , 115  , 109  , 104 ,
  98  , 92  , 86  , 80  , 74  , 68  , 62  , 56  , 50  , 44  , 38  , 31  , 25  , 19  , 13  , 6 ,
  0  , -6  , -13  , -19  , -25  , -31  , -38  , -44  , -50  , -56  , -62  , -68  , -74  , -80  , -86  , -92,
  - 98  , -104  , -109  , -115  , -121  , -126  , -132  , -137  , -142  , -147  , -152  , -157  , -162  , -167  , -172  , -177,
  - 181  , -185  , -190  , -194  , -198  , -202  , -206  , -209  , -213  , -216  , -220  , -223  , -226  , -229  , -231  , -234 ,
  - 237  , -239  , -241  , -243  , -245  , -247  , -248  , -250  , -251  , -252  , -253  , -254  , -255  , -255  , -256  , -256
};
int const im_wn[128] PROGMEM = {                                                     //imaginäre Wn Values  (Sin)
  0  , -6  , -13  , -19  , -25  , -31  , -38  , -44  , -50  , -56  , -62  , -68  , -74  , -80  , -86  , -92,
  - 98  , -104  , -109  , -115  , -121  , -126  , -132  , -137  , -142  , -147  , -152  , -157  , -162  , -167  , -172  , -177,
  - 181  , -185  , -190  , -194  , -198  , -202  , -206  , -209  , -213  , -216  , -220  , -223  , -226  , -229  , -231  , -234,
  - 237  , -239  , -241  , -243  , -245  , -247  , -248  , -250  , -251  , -252  , -253  , -254  , -255  , -255  , -256  , -256,
  - 256  , -256  , -256  , -255  , -255  , -254  , -253  , -252  , -251  , -250  , -248  , -247  , -245  , -243  , -241  , -239,
  - 237  , -234  , -231  , -229  , -226  , -223  , -220  , -216  , -213  , -209  , -206  , -202  , -198  , -194  , -190  , -185,
  - 181  , -177  , -172  , -167  , -162  , -157  , -152  , -147  , -142  , -137  , -132  , -126  , -121  , -115  , -109  , -104,
  - 98  , -92  , -86  , -80  , -74  , -68  , -62  , -56  , -50  , -44  , -38  , -31  , -25  , -19  , -13  , -6
};
//------------------------------------------------------------------------------------------------------------------
//256 Points Dft shared in 128*2 Points Dft
//1.Dft =0/64  2.=32/96  etc.              K = 0-N/2
//------------------------------------------------------------------------------------------------------------------
byte const dft[2500] PROGMEM = {
  0 , 128 , 64 , 192 , 32 , 160 , 96 , 224 , 16 , 144 , 80 , 208 , 48 , 176 , 112 , 240 ,      //start adr.0 Dec
  8 , 136 , 72 , 200 , 40 , 168 , 104 , 232 , 24 , 152 , 88 , 216 , 56 , 184 , 120 , 248 ,
  4 , 132 , 68 , 196 , 36 , 164 , 100 , 228 , 20 , 148 , 84 , 212 , 52 , 180 , 116 , 244,
  12 , 140 , 76 , 204 , 44 , 172 , 108 , 236 , 28 , 156 , 92 , 220 , 60 , 188 , 124 , 252,
  2 , 130 , 66 , 194 , 34 , 162 , 98 , 226 , 18 , 146 , 82 , 210 , 50 , 178 , 114 , 242,
  10 , 138 , 74 , 202 , 42 , 170 , 106 , 234 , 26 , 154 , 90 , 218 , 58 , 186 , 122 , 250,
  6 , 134 , 70 , 198 , 38 , 166 , 102 , 230 , 22 , 150 , 86 , 214 , 54 , 182 , 118 , 246,
  14 , 142 , 78 , 206 , 46 , 174 , 110 , 238 , 30 , 158 , 94 , 222 , 62 , 190 , 126 , 254,
  1 , 129 , 65 , 193 , 33 , 161 , 97 , 225 , 17 , 145 , 81 , 209 , 49 , 177 , 113 , 241,
  9 , 137 , 73 , 201 , 41 , 169 , 105 , 233 , 25 , 153 , 89 , 217 , 57 , 185 , 121 , 249,
  5 , 133 , 69 , 197 , 37 , 165 , 101 , 229 , 21 , 149 , 85 , 213 , 53 , 181 , 117 , 245,
  13 , 141 , 77 , 205 , 45 , 173 , 109 , 237 , 29 , 157 , 93 , 221 , 61 , 189 , 125 , 253,
  3 , 131 , 67 , 195 , 35 , 163 , 99 , 227 , 19 , 147 , 83 , 211 , 51 , 179 , 115 , 243,
  11 , 139 , 75 , 203 , 43 , 171 , 107 , 235 , 27 , 155 , 91 , 219 , 59 , 187 , 123 , 251,
  7 , 135 , 71 , 199 , 39 , 167 , 103 , 231 , 23 , 151 , 87 , 215 , 55 , 183 , 119 , 247,
  15 , 143 , 79 , 207 , 47 , 175 , 111 , 239 , 31 , 159 , 95 , 223 , 63 , 191 , 127 , 255,
  //------------------------------------------------------------------------------------------------------------------
  0 , 64 , 128 , 192 , 32 , 96 , 160 , 224 , 16 , 80 , 144 , 208 , 48 , 112 , 176 , 240  ,     //start adr.256 Dec
  8 , 72 , 136 , 200 , 40 , 104 , 168 , 232 , 24 , 88 , 152 , 216 , 56 , 120 , 184 , 248,
  4 , 68 , 132 , 196 , 36 , 100 , 164 , 228 , 20 , 84 , 148 , 212 , 52 , 116 , 180 , 244,
  12 , 76 , 140 , 204 , 44 , 108 , 172 , 236 , 28 , 92 , 156 , 220 , 60 , 124 , 188 , 252,
  2 , 66 , 130 , 194 , 34 , 98 , 162 , 226 , 18 , 82 , 146 , 210 , 50 , 114 , 178 , 242,
  10 , 74 , 138 , 202 , 42 , 106 , 170 , 234 , 26 , 90 , 154 , 218 , 58 , 122 , 186 , 250,
  6 , 70 , 134 , 198 , 38 , 102 , 166 , 230 , 22 , 86 , 150 , 214 , 54 , 118 , 182 , 246,
  14 , 78 , 142 , 206 , 46 , 110 , 174 , 238 , 30 , 94 , 158 , 222 , 62 , 126 , 190 , 254,
  1 , 65 , 129 , 193 , 33 , 97 , 161 , 225 , 17 , 81 , 145 , 209 , 49 , 113 , 177 , 241,
  9 , 73 , 137 , 201 , 41 , 105 , 169 , 233 , 25 , 89 , 153 , 217 , 57 , 121 , 185 , 249,
  5 , 69 , 133 , 197 , 37 , 101 , 165 , 229 , 21 , 85 , 149 , 213 , 53 , 117 , 181 , 245,
  13 , 77 , 141 , 205 , 45 , 109 , 173 , 237 , 29 , 93 , 157 , 221 , 61 , 125 , 189 , 253,
  3 , 67 , 131 , 195 , 35 , 99 , 163 , 227 , 19 , 83 , 147 , 211 , 51 , 115 , 179 , 243,
  11 , 75 , 139 , 203 , 43 , 107 , 171 , 235 , 27 , 91 , 155 , 219 , 59 , 123 , 187 , 251,
  7 , 71 , 135 , 199 , 39 , 103 , 167 , 231 , 23 , 87 , 151 , 215 , 55 , 119 , 183 , 247,
  15 , 79 , 143 , 207 , 47 , 111 , 175 , 239 , 31 , 95 , 159 , 223 , 63 , 127 , 191 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 32 , 128 , 160 , 64 , 96 , 192 , 224 , 16 , 48 , 144 , 176 , 80 , 112 , 208 , 240   ,    //start adr.512 Dec
  8 , 40 , 136 , 168 , 72 , 104 , 200 , 232 , 24 , 56 , 152 , 184 , 88 , 120 , 216 , 248,
  4 , 36 , 132 , 164 , 68 , 100 , 196 , 228 , 20 , 52 , 148 , 180 , 84 , 116 , 212 , 244,
  12 , 44 , 140 , 172 , 76 , 108 , 204 , 236 , 28 , 60 , 156 , 188 , 92 , 124 , 220 , 252,
  2 , 34 , 130 , 162 , 66 , 98 , 194 , 226 , 18 , 50 , 146 , 178 , 82 , 114 , 210 , 242,
  10 , 42 , 138 , 170 , 74 , 106 , 202 , 234 , 26 , 58 , 154 , 186 , 90 , 122 , 218 , 250,
  6 , 38 , 134 , 166 , 70 , 102 , 198 , 230 , 22 , 54 , 150 , 182 , 86 , 118 , 214 , 246,
  14 , 46 , 142 , 174 , 78 , 110 , 206 , 238 , 30 , 62 , 158 , 190 , 94 , 126 , 222 , 254,
  1 , 33 , 129 , 161 , 65 , 97 , 193 , 225 , 17 , 49 , 145 , 177 , 81 , 113 , 209 , 241,
  9 , 41 , 137 , 169 , 73 , 105 , 201 , 233 , 25 , 57 , 153 , 185 , 89 , 121 , 217 , 249,
  5 , 37 , 133 , 165 , 69 , 101 , 197 , 229 , 21 , 53 , 149 , 181 , 85 , 117 , 213 , 245,
  13 , 45 , 141 , 173 , 77 , 109 , 205 , 237 , 29 , 61 , 157 , 189 , 93 , 125 , 221 , 253,
  3 , 35 , 131 , 163 , 67 , 99 , 195 , 227 , 19 , 51 , 147 , 179 , 83 , 115 , 211 , 243,
  11 , 43 , 139 , 171 , 75 , 107 , 203 , 235 , 27 , 59 , 155 , 187 , 91 , 123 , 219 , 251,
  7 , 39 , 135 , 167 , 71 , 103 , 199 , 231 , 23 , 55 , 151 , 183 , 87 , 119 , 215 , 247,
  15 , 47 , 143 , 175 , 79 , 111 , 207 , 239 , 31 , 63 , 159 , 191 , 95 , 127 , 223 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 16 , 128 , 144 , 64 , 80 , 192 , 208 , 32 , 48 , 160 , 176 , 96 , 112 , 224 , 240  ,     //start adr.768 Dec
  8 , 24 , 136 , 152 , 72 , 88 , 200 , 216 , 40 , 56 , 168 , 184 , 104 , 120 , 232 , 248,
  4 , 20 , 132 , 148 , 68 , 84 , 196 , 212 , 36 , 52 , 164 , 180 , 100 , 116 , 228 , 244,
  12 , 28 , 140 , 156 , 76 , 92 , 204 , 220 , 44 , 60 , 172 , 188 , 108 , 124 , 236 , 252,
  2 , 18 , 130 , 146 , 66 , 82 , 194 , 210 , 34 , 50 , 162 , 178 , 98 , 114 , 226 , 242,
  10 , 26 , 138 , 154 , 74 , 90 , 202 , 218 , 42 , 58 , 170 , 186 , 106 , 122 , 234 , 250,
  6 , 22 , 134 , 150 , 70 , 86 , 198 , 214 , 38 , 54 , 166 , 182 , 102 , 118 , 230 , 246,
  14 , 30 , 142 , 158 , 78 , 94 , 206 , 222 , 46 , 62 , 174 , 190 , 110 , 126 , 238 , 254,
  1 , 17 , 129 , 145 , 65 , 81 , 193 , 209 , 33 , 49 , 161 , 177 , 97 , 113 , 225 , 241,
  9 , 25 , 137 , 153 , 73 , 89 , 201 , 217 , 41 , 57 , 169 , 185 , 105 , 121 , 233 , 249,
  5 , 21 , 133 , 149 , 69 , 85 , 197 , 213 , 37 , 53 , 165 , 181 , 101 , 117 , 229 , 245,
  13 , 29 , 141 , 157 , 77 , 93 , 205 , 221 , 45 , 61 , 173 , 189 , 109 , 125 , 237 , 253,
  3 , 19 , 131 , 147 , 67 , 83 , 195 , 211 , 35 , 51 , 163 , 179 , 99 , 115 , 227 , 243,
  11 , 27 , 139 , 155 , 75 , 91 , 203 , 219 , 43 , 59 , 171 , 187 , 107 , 123 , 235 , 251,
  7 , 23 , 135 , 151 , 71 , 87 , 199 , 215 , 39 , 55 , 167 , 183 , 103 , 119 , 231 , 247,
  15 , 31 , 143 , 159 , 79 , 95 , 207 , 223 , 47 , 63 , 175 , 191 , 111 , 127 , 239 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 8 , 128 , 136 , 64 , 72 , 192 , 200 , 32 , 40 , 160 , 168 , 96 , 104 , 224 , 232,       //start adr.1024 Dec
  16 , 24 , 144 , 152 , 80 , 88 , 208 , 216 , 48 , 56 , 176 , 184 , 112 , 120 , 240 , 248,
  4 , 12 , 132 , 140 , 68 , 76 , 196 , 204 , 36 , 44 , 164 , 172 , 100 , 108 , 228 , 236,
  20 , 28 , 148 , 156 , 84 , 92 , 212 , 220 , 52 , 60 , 180 , 188 , 116 , 124 , 244 , 252,
  2 , 10 , 130 , 138 , 66 , 74 , 194 , 202 , 34 , 42 , 162 , 170 , 98 , 106 , 226 , 234,
  18 , 26 , 146 , 154 , 82 , 90 , 210 , 218 , 50 , 58 , 178 , 186 , 114 , 122 , 242 , 250,
  6 , 14 , 134 , 142 , 70 , 78 , 198 , 206 , 38 , 46 , 166 , 174 , 102 , 110 , 230 , 238,
  22 , 30 , 150 , 158 , 86 , 94 , 214 , 222 , 54 , 62 , 182 , 190 , 118 , 126 , 246 , 254,
  1 , 9 , 129 , 137 , 65 , 73 , 193 , 201 , 33 , 41 , 161 , 169 , 97 , 105 , 225 , 233,
  17 , 25 , 145 , 153 , 81 , 89 , 209 , 217 , 49 , 57 , 177 , 185 , 113 , 121 , 241 , 249,
  5 , 13 , 133 , 141 , 69 , 77 , 197 , 205 , 37 , 45 , 165 , 173 , 101 , 109 , 229 , 237,
  21 , 29 , 149 , 157 , 85 , 93 , 213 , 221 , 53 , 61 , 181 , 189 , 117 , 125 , 245 , 253,
  3 , 11 , 131 , 139 , 67 , 75 , 195 , 203 , 35 , 43 , 163 , 171 , 99 , 107 , 227 , 235,
  19 , 27 , 147 , 155 , 83 , 91 , 211 , 219 , 51 , 59 , 179 , 187 , 115 , 123 , 243 , 251,
  7 , 15 , 135 , 143 , 71 , 79 , 199 , 207 , 39 , 47 , 167 , 175 , 103 , 111 , 231 , 239,
  23 , 31 , 151 , 159 , 87 , 95 , 215 , 223 , 55 , 63 , 183 , 191 , 119 , 127 , 247 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 4 , 128 , 132 , 64 , 68 , 192 , 196 , 32 , 36 , 160 , 164 , 96 , 100 , 224 , 228  ,     //start adr.1280 Dec
  16 , 20 , 144 , 148 , 80 , 84 , 208 , 212 , 48 , 52 , 176 , 180 , 112 , 116 , 240 , 244,
  8 , 12 , 136 , 140 , 72 , 76 , 200 , 204 , 40 , 44 , 168 , 172 , 104 , 108 , 232 , 236,
  24 , 28 , 152 , 156 , 88 , 92 , 216 , 220 , 56 , 60 , 184 , 188 , 120 , 124 , 248 , 252,
  2 , 6 , 130 , 134 , 66 , 70 , 194 , 198 , 34 , 38 , 162 , 166 , 98 , 102 , 226 , 230,
  18 , 22 , 146 , 150 , 82 , 86 , 210 , 214 , 50 , 54 , 178 , 182 , 114 , 118 , 242 , 246,
  10 , 14 , 138 , 142 , 74 , 78 , 202 , 206 , 42 , 46 , 170 , 174 , 106 , 110 , 234 , 238,
  26 , 30 , 154 , 158 , 90 , 94 , 218 , 222 , 58 , 62 , 186 , 190 , 122 , 126 , 250 , 254,
  1 , 5 , 129 , 133 , 65 , 69 , 193 , 197 , 33 , 37 , 161 , 165 , 97 , 101 , 225 , 229,
  17 , 21 , 145 , 149 , 81 , 85 , 209 , 213 , 49 , 53 , 177 , 181 , 113 , 117 , 241 , 245,
  9 , 13 , 137 , 141 , 73 , 77 , 201 , 205 , 41 , 45 , 169 , 173 , 105 , 109 , 233 , 237,
  25 , 29 , 153 , 157 , 89 , 93 , 217 , 221 , 57 , 61 , 185 , 189 , 121 , 125 , 249 , 253,
  3 , 7 , 131 , 135 , 67 , 71 , 195 , 199 , 35 , 39 , 163 , 167 , 99 , 103 , 227 , 231,
  19 , 23 , 147 , 151 , 83 , 87 , 211 , 215 , 51 , 55 , 179 , 183 , 115 , 119 , 243 , 247,
  11 , 15 , 139 , 143 , 75 , 79 , 203 , 207 , 43 , 47 , 171 , 175 , 107 , 111 , 235 , 239,
  27 , 31 , 155 , 159 , 91 , 95 , 219 , 223 , 59 , 63 , 187 , 191 , 123 , 127 , 251 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 2 , 128 , 130 , 64 , 66 , 192 , 194 , 32 , 34 , 160 , 162 , 96 , 98 , 224 , 226 ,      //start adr.1536 Dec
  16 , 18 , 144 , 146 , 80 , 82 , 208 , 210 , 48 , 50 , 176 , 178 , 112 , 114 , 240 , 242,
  8 , 10 , 136 , 138 , 72 , 74 , 200 , 202 , 40 , 42 , 168 , 170 , 104 , 106 , 232 , 234,
  24 , 26 , 152 , 154 , 88 , 90 , 216 , 218 , 56 , 58 , 184 , 186 , 120 , 122 , 248 , 250,
  4 , 6 , 132 , 134 , 68 , 70 , 196 , 198 , 36 , 38 , 164 , 166 , 100 , 102 , 228 , 230,
  20 , 22 , 148 , 150 , 84 , 86 , 212 , 214 , 52 , 54 , 180 , 182 , 116 , 118 , 244 , 246,
  12 , 14 , 140 , 142 , 76 , 78 , 204 , 206 , 44 , 46 , 172 , 174 , 108 , 110 , 236 , 238,
  28 , 30 , 156 , 158 , 92 , 94 , 220 , 222 , 60 , 62 , 188 , 190 , 124 , 126 , 252 , 254,
  1 , 3 , 129 , 131 , 65 , 67 , 193 , 195 , 33 , 35 , 161 , 163 , 97 , 99 , 225 , 227,
  17 , 19 , 145 , 147 , 81 , 83 , 209 , 211 , 49 , 51 , 177 , 179 , 113 , 115 , 241 , 243,
  9 , 11 , 137 , 139 , 73 , 75 , 201 , 203 , 41 , 43 , 169 , 171 , 105 , 107 , 233 , 235,
  25 , 27 , 153 , 155 , 89 , 91 , 217 , 219 , 57 , 59 , 185 , 187 , 121 , 123 , 249 , 251,
  5 , 7 , 133 , 135 , 69 , 71 , 197 , 199 , 37 , 39 , 165 , 167 , 101 , 103 , 229 , 231,
  21 , 23 , 149 , 151 , 85 , 87 , 213 , 215 , 53 , 55 , 181 , 183 , 117 , 119 , 245 , 247,
  13 , 15 , 141 , 143 , 77 , 79 , 205 , 207 , 45 , 47 , 173 , 175 , 109 , 111 , 237 , 239,
  29 , 31 , 157 , 159 , 93 , 95 , 221 , 223 , 61 , 63 , 189 , 191 , 125 , 127 , 253 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  0 , 1 , 128 , 129 , 64 , 65 , 192 , 193 , 32 , 33 , 160 , 161 , 96 , 97 , 224 , 225 ,      //start adr.1792 Dec
  16 , 17 , 144 , 145 , 80 , 81 , 208 , 209 , 48 , 49 , 176 , 177 , 112 , 113 , 240 , 241,
  8 , 9 , 136 , 137 , 72 , 73 , 200 , 201 , 40 , 41 , 168 , 169 , 104 , 105 , 232 , 233,
  24 , 25 , 152 , 153 , 88 , 89 , 216 , 217 , 56 , 57 , 184 , 185 , 120 , 121 , 248 , 249,
  4 , 5 , 132 , 133 , 68 , 69 , 196 , 197 , 36 , 37 , 164 , 165 , 100 , 101 , 228 , 229,
  20 , 21 , 148 , 149 , 84 , 85 , 212 , 213 , 52 , 53 , 180 , 181 , 116 , 117 , 244 , 245,
  12 , 13 , 140 , 141 , 76 , 77 , 204 , 205 , 44 , 45 , 172 , 173 , 108 , 109 , 236 , 237,
  28 , 29 , 156 , 157 , 92 , 93 , 220 , 221 , 60 , 61 , 188 , 189 , 124 , 125 , 252 , 253,
  2 , 3 , 130 , 131 , 66 , 67 , 194 , 195 , 34 , 35 , 162 , 163 , 98 , 99 , 226 , 227,
  18 , 19 , 146 , 147 , 82 , 83 , 210 , 211 , 50 , 51 , 178 , 179 , 114 , 115 , 242 , 243,
  10 , 11 , 138 , 139 , 74 , 75 , 202 , 203 , 42 , 43 , 170 , 171 , 106 , 107 , 234 , 235,
  26 , 27 , 154 , 155 , 90 , 91 , 218 , 219 , 58 , 59 , 186 , 187 , 122 , 123 , 250 , 251,
  6 , 7 , 134 , 135 , 70 , 71 , 198 , 199 , 38 , 39 , 166 , 167 , 102 , 103 , 230 , 231,
  22 , 23 , 150 , 151 , 86 , 87 , 214 , 215 , 54 , 55 , 182 , 183 , 118 , 119 , 246 , 247,
  14 , 15 , 142 , 143 , 78 , 79 , 206 , 207 , 46 , 47 , 174 , 175 , 110 , 111 , 238 , 239,
  30 , 31 , 158 , 159 , 94 , 95 , 222 , 223 , 62 , 63 , 190 , 191 , 126 , 127 , 254 , 255,
  //-------------------------------------------------------------------------------------------------------------------
  //wn_table[288] //Wn                                                                       //start Adr:   2048=800hex
  0 , 64 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0   ,                         //Dec  2048
  0 , 32 , 64 , 96 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0  ,                        //Dec  2064
  0 , 16 , 32 , 48 , 64 , 80 , 96 , 112 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,                     //Dec  2080
  0 , 8 , 16 , 24 , 32 , 40 , 48 , 56 , 64 , 72 , 80 , 88 , 96 , 104 , 112 , 120,            //Dec  2096
  0 , 4 , 8 , 12 , 16 , 20 , 24 , 28 , 32 , 36 , 40 , 44 , 48 , 52 , 56 , 60     ,           //Dec  2112
  64 , 68 , 72 , 76 , 80 , 84 , 88 , 92 , 96 , 100 , 104 , 108 , 112 , 116 , 120 , 124,
  0 , 2 , 4 , 6 , 8 , 10 , 12 , 14 , 16 , 18 , 20 , 22 , 24 , 26 , 28 , 30 ,                 //Dec  2144
  32 , 34 , 36 , 38 , 40 , 42 , 44 , 46 , 48 , 50 , 52 , 54 , 56 , 58 , 60 , 62,
  64 , 66 , 68 , 70 , 72 , 74 , 76 , 78 , 80 , 82 , 84 , 86 , 88 , 90 , 92 , 94,
  96 , 98 , 100 , 102 , 104 , 106 , 108 , 110 , 112 , 114 , 116 , 118 , 120 , 122 , 124 , 126,
  0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 , 11 , 12 , 13 , 14 , 15 ,                      //Dec 2208
  16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 , 25 , 26 , 27 , 28 , 29 , 30 , 31,
  32 , 33 , 34 , 35 , 36 , 37 , 38 , 39 , 40 , 41 , 42 , 43 , 44 , 45 , 46 , 47,
  48 , 49 , 50 , 51 , 52 , 53 , 54 , 55 , 56 , 57 , 58 , 59 , 60 , 61 , 62 , 63,
  64 , 65 , 66 , 67 , 68 , 69 , 70 , 71 , 72 , 73 , 74 , 75 , 76 , 77 , 78 , 79,
  80 , 81 , 82 , 83 , 84 , 85 , 86 , 87 , 88 , 89 , 90 , 91 , 92 , 93 , 94 , 95,
  96 , 97 , 98 , 99 , 100 , 101 , 102 , 103 , 104 , 105 , 106 , 107 , 108 , 109 , 110 , 111,
  112 , 113 , 114 , 115 , 116 , 117 , 118 , 119 , 120 , 121 , 122 , 123 , 124 , 125 , 126 , 127
};
//-------------------------------------------------------------------------------------------------------------------

int A[256];                             //Sample array
int Ak[256];                            //Reell array
int Bk[256];                            //Imaginär array
byte P[128];                            //Frequency Spectrum output

byte t;
byte y;
byte x;
byte w;
int z;
int s;
int u;
int v;

int zw1;
int zw2;
int zw3;
int zw4;
int zw5;
int zw6;
int zw7;
int zw8;

#define led 13
#define controll_pin 12
#define analog_pin 0
boolean led_status = false;
int counter = 0;
//-----------------------------------------------------------------------------------------
void setup() {

  pinMode(led, OUTPUT);
  pinMode(controll_pin, OUTPUT);
  pinMode(analog_pin, INPUT);

  Serial.begin(9600);
  check();
  //load_sinus();
  //load_rectangle();
  load_triangle();
  //load_zero();
  fft();
  koeffizienten();
  output_terminal();

  Timer1.initialize(microseconds_1);
  Timer1.attachInterrupt(timer1_subroutine);
}
//-----------------------------------------------------------------------------------------
void loop() {

  fft();
  koeffizienten();
  output_led();
  if (led_status == true) {
    led_status = false;
    digitalWrite(led, HIGH); //  Toggle led
  }
  else {
    led_status = true;
    digitalWrite(led, LOW);
  }
}
//-----------------------------------------------------------------------------------------
void check() {                                                 //EEprom check

  zw1 = 0;
  for (int i = 0 ; i < 256; i++) {                             //32640 Checksumme
    zw1 += pgm_read_byte_near(dft + (i + 1792));
    //Serial.print(String(pgm_read_byte_near(dft + (i+1792))) + ",");
  }
  Serial.print(String(zw1));
  if (zw1 == 32640) Serial.println(" > EEprom Check OK");
  if (zw1 != 32640) Serial.println(" > EEprom Check Failed");

}
//-----------------------------------------------------------------------------------------
void load_sinus() {                                              //Test-Sinus

  float pi_2 = 6.28 / 256;
  for (int i = 0 ; i < 256; i++) {
    A[i] = int(sin(pi_2 * i) * 50);
    //Serial.println(String(A[i]));
  }
  Serial.println("Sinus loaded");
}
//-----------------------------------------------------------------------------------------
void load_zero() {                                              //Test-Zero line

  for (int i = 0 ; i < 256; i++) {
    A[i] = 50;
    //Serial.println(String(A[i]));
  }
  Serial.println("Zero Line loaded");

}
//-----------------------------------------------------------------------------------------
void load_rectangle() {                                              //Test-Rectangle line

  for (int i = 0 ; i < 128; i++) {
    A[i] = 50;
    //Serial.println(String(A[i]));
  }
  for (int i = 128 ; i < 256; i++) {
    A[i] = -50;
    //Serial.println(String(A[i]));
  }
  Serial.println("Rectangle loaded");
}
//---------------------------------------------------------------------------------------- -
void load_triangle() {                                              //Test-Rectangle line

  for (int i = 0 ; i < 64; i++) {
    A[i] = i;
    //Serial.println(String(A[i]));
  }
  for (int i = 64 ; i < 128; i++) {
    A[i] = 128 - i;
    //Serial.println(String(A[i]));
  }
  for (int i = 128 ; i < 192; i++) {
    A[i] = (128 - i);
    //Serial.println(String(A[i]));
  }
  for (int i = 192 ; i < 256; i++) {
    A[i] = i - 256;
    //Serial.println(String(A[i]));
  }
  Serial.println("Triangle loaded");
}
//-----------------------------------------------------------------------------------------
void division_zw1_256() {
  //zw1 = zw1 / 256;
  zw1 = zw1 >> 8;
}
//-----------------------------------------------------------------------------------------
void division_zw2_256() {
  //zw2 = zw2 / 256;
  zw2 = zw2 >> 8;
}
//-----------------------------------------------------------------------------------------
void division_zw3_256() {
  //zw3 = zw3 / 256;
  zw3 = zw3 >> 8;
}
//-----------------------------------------------------------------------------------------
void division_zw4_256() {
  //zw4 = zw4 / 256;
  zw4 = zw4 >> 8;
}
//-----------------------------------------------------------------------------------------
void division_zw1_2() {
  //zw1 = zw1 / 2;
  zw1 = zw1 >> 1;
}
//-----------------------------------------------------------------------------------------
void division_zw2_2() {
  //zw2 = zw2 / 2;
  zw2 = zw2 >> 1;
}
//-----------------------------------------------------------------------------------------
void division_zw3_2() {
  //zw3 = zw3 / 2;
  zw3 = zw3 >> 1;
}
//-----------------------------------------------------------------------------------------
void fft() {                                                //FFT

  z = 0;
  do {
    x = pgm_read_byte_near(dft + z);
    zw1 = A[x];
    z++;
    y = pgm_read_byte_near(dft + z);                         //2.Punkte DFT
    zw2 = A[y];
    z++;
    //-------------------
    zw3 = zw1 + zw2;                                         //1.Rücksortierung
    division_zw3_2();                                        //zw3 = zw3 / 2
    Ak[x] = zw3;
    Bk[x] = 0;
    zw3 = zw1 - zw2;
    division_zw3_2()  ;                                      //zw3 = zw3 / 2
    Ak[y] = zw3;
    Bk[y] = 0;
  } while (z < 256);

  //-----------------
  z = 256;                                                   //2. Rücksortierung
  v = 2048;
  t = 2;                                                     //2 mal W4^0-W4^1
  s = 2050;
  transformation();
  //-----------------
  z = 512;                                                   //3. Rücksortierung
  v = 2064;
  t = 4;                                                     //4 mal W8^0-W8^3
  s = 2068;
  transformation();
  //-----------------
  z = 768;                                                   //4. Rücksortierung
  v = 2080;
  t = 8;                                                     //8 mal W16^0-W16^7
  s = 2088;
  transformation();
  //-----------------
  z = 1024;                                                  //5. Rücksortierung
  v = 2096;
  t = 16;                                                    //16 mal  W32^0-W32^15
  s = 2112;
  transformation();
  //-----------------
  z = 1280;                                                  //6. Rücksortierung
  v = 2112;
  t = 32;                                                    //32 mal  W64^0-W64^31
  s = 2144;
  transformation();
  //-----------------
  z = 1536 ;                                                 //7. Rücksortierung
  v = 2144;
  t = 64;                                                    //64 mal W128^0-W128^63
  s = 2208;
  transformation();
  //-----------------
  z = 1792;                                                  //8. Rücksortierung
  v = 2208;
  t = 128;                                                   //128 mal W256^0-W256^127
  s = 2336;
  transformation();
}
//-----------------------------------------------------------------------------------------
void transformation() {
  //                                                         //1.EEprom Address for the Butterfly array
  //                                                         //loop for 128 Butterflys
  for (int u = 0; u < 128; u++) {

    x = pgm_read_byte_near(dft + z);
    z++;                                                    //next EEpromaddress
    y = pgm_read_byte_near(dft + z);
    z++;                                                    //next EEpromaddress

    w = pgm_read_byte_near(dft + v);                                              //1.Wn Factor is ever Wn^0 and located at address Re[0] und Im[0]
    zw1 = Ak[y] * pgm_read_word_near(re_wn + w);
    division_zw1_256();                                      //zw1=zw1/256
    zw2 = Bk[y] * pgm_read_word_near(im_wn + w);
    division_zw2_256();                                      //zw2 = zw2 / 256
    zw3 = Ak[y] * pgm_read_word_near(im_wn + w);
    division_zw3_256();                                      //zw3 = zw3 / 256
    zw4 = Bk[y] * pgm_read_word_near(re_wn + w);
    division_zw4_256();                                      //zw4 = zw4 / 256

    zw5 = zw1 - zw2;                                         //zw5 = Ak[y]
    zw6 = zw3 + zw4;                                         //zw6 = Bk[y]
    zw7 = Ak[x];                                             //zw7 = Ak[x]
    zw8 = Bk[x];                                             //zw8 = Bk[x]

    //copy back to Ak[x] and Bk[x] , Ak[y] and Bk[y],
    zw1 = zw7 + zw5;                                         //Butterfly Operations
    division_zw1_2();
    Ak[x] = zw1;

    zw2 = zw8 + zw6;
    division_zw2_2();
    Bk[x] = zw2;

    zw3 = zw7 - zw5;
    division_zw3_2();
    Ak[y] = zw3;

    zw3 = zw8 - zw6;
    division_zw3_2();
    Bk[y] = zw3;

    v++;                                                    //next Wn value
    if ( v = s ) v = s - t;
  }
}
//-----------------------------------------------------------------------------------------
void koeffizienten() {

  for (int i = 0 ; i < 128; i++) {
    x = pgm_read_byte_near(dft + i);
    zw1 = Ak[x];
    zw2 = Bk[x];
    zw1 = abs(zw1);                                            //quick Pythagoras :-)
    zw2 = abs(zw2);
    if (zw1 < zw2) {
      zw2 = zw2 + zw2;
    }
    else {
      zw1 = zw1 + zw1;
    }
    zw3 = zw1 + zw2;
    P[i] = zw3;
  }
}
//-----------------------------------------------------------------------------------------
void output_terminal() {

  for (int i = 0 ; i < 128; i++) {
    Serial.println(String(P[i]) + ",");
  }
}
//-----------------------------------------------------------------------------------------
void output_led() {

  for (int i = 0 ; i < 128; i++) {
    //do something with led
  }
}
//-----------------------------------------------------------------------------------------
void timer1_subroutine(void) {

  digitalWrite(controll_pin, HIGH); //measure the Sample frequenz
  digitalWrite(controll_pin, LOW);
  A[counter] = analogRead(analog_pin) - 512;  // read the input pin - 512
  counter++;
  if (counter > 255) counter = 0;

}
