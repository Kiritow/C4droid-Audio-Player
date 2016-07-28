/** Cross-Platform Audio Player
 * Created By HC TECH
 * [CPPLIB OpenSource Project]
 * HC TECH 2016
*/
#ifndef cpplib_audio_player
#define cpplib_audio_player

#define AUDIO_PLAYING 1
#define AUDIO_PAUSE 2
#define AUDIO_STOP 3

#if defined( __WIN32__ ) || !defined(__C4DROID__)

#include "win_player.hpp"

#else /// C4droid

#include "c4d_player.hpp"

#endif /// End of cpplib_audio_player
