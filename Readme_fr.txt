//------------------------------------------------------------------
// Daídalos Démo Alpha 2
//------------------------------------------------------------------
// Type: FPS 3D
// Langage: C++
// Bibliothèques: DirectX, Direct Input, OpenAL
// En projet depuis: Juillet 2008
// Site: http://www.daidalos-game.com
//------------------------------------------------------------------

//-------------------------------------------------------------------
// Changelog
//-------------------------------------------------------------------
// Build 2011.02.25:
//  Correction des plantages lors de la perte du focus [Bug #8]
//  Correction de la synchro verticale [Bug #9]
//  Ajout de la gestion du son 3D
//  Ajout de sons supplémentaires pour compléter le niveau test
//
// Build 2011.02.14:
//  Modification du fov vertical (45° -> 55°)
//  Ajout des ombres pour le rendu Forward [Bug #6]
//  Ajout des résolutions compatibles en mode fenêtré [Bug #7]
//  Correction d'un bug de sensibilité de la souris et de la manette.
//  Ajout d'un début de gestion de sons et de musiques.
//
// Build 2011.02.11:
//  Première sortie. Il manque beaucoup de choses, mais le principal
//  but de cette sortie est de donner la possibilité de tester le
//  jeu tôt dans le développement.
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Remerciements
//-------------------------------------------------------------------
//+NICS pour son tuto sur les arbres bsp (http://nics.developpez.com/tutoriels/3D/physique/glissement-forme-contre-bsp/)
// qui m'a permi de mettre en place les collisions.
// 
//+Les sons:
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
//+Musiques
// Kevin MacLeod (http://www.incompetech.com)
//
//+Les textures
// http://www.cgtextures.com/
//-------------------------------------------------------------------
