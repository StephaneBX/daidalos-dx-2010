//------------------------------------------------------------------
// Daídalos Alpha 2 Demo
//------------------------------------------------------------------
// Type: 3D FPS
// Language: C++
// Libraries: DirectX, Direct Input, OpenAL
// In progress since: Jully, 2008
// Web site: http://www.daidalos-game.com
//------------------------------------------------------------------

//-------------------------------------------------------------------
// Changelog
//-------------------------------------------------------------------
// Build 2011.02.25:
//  Corrected the crashes when the application loses the focus [Bug #8]
//  Corrected the vertical synchronization problems [Bug #9]
//  Added 3D sounds management.
//  Added sounds to complete the test map.
//
// Build 2011.02.14:
//  Vertical fov modification (45° -> 55°)
//  Added shadows for Foward Rendering [Bug #6]
//  Added windowed compatible resolutions [Bug #7]
//  Corrected a bug when modifying mouse or gamepad sensibility values.
//  Added a beginning of sounds and musics management.
//
// Build 2011.02.11:
//  First release. A lot is missing but the main goal of this build
//  is to give the possibilty for the user to test the game early.
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Thanks
//-------------------------------------------------------------------
//+NICS for his bsp tree tutorial (http://nics.developpez.com/tutoriels/3D/physique/glissement-forme-contre-bsp/)
// that helped me seting up the collision system.
//
//+Sounds:
// Flick3r
// dynamedion (http://www.dynamedion.com)
// roocks (http://www.rookiecop.co.uk)
// Airborne Sound (http://www.airbornesound.com)
// SFX Bible (http://www.soundeffectsbible.com)
// BlastwaveFX (http://www.blastwavefx.com/)
// Shriek Productions (http://www.shriek-music.com/)
// Justine Angus
// Big Room Sound (http://www.bigroomsound.com/Home.html)
//
//+Musics
// Kevin MacLeod (http://www.incompetech.com)
//
//+Textures
// http://www.cgtextures.com/
//-------------------------------------------------------------------
