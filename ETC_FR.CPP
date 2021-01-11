/*
** Astrolog File: etc_fr.c
**
** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1998 by Walter D. Pullen
** (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the library SWISS EPHEMERIS and are programmed and
** copyright 1998 by Astrodienst AG.
** The use of that source code is subject to
** the Swiss Ephemeris Public License, available at 
** http://www.astro.ch/swisseph. This copyright notice must not be 
** changed or removed by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 12/20/1998.
** Modifications from version 5.40 to 5.41 are by Alois Treindl.
** Modifications from version 5.41 to 5.41g are by Valentin Abramov.
** Windows 32-bits version by Antonio Ramos.
**
** This software is provided "as is", without warranty of any kind, either 
** express or implied. In no event shall the authors or copyright holders be 
** liable for any claim, damages or other liability, arising from the use or 
** performance of this software.
**
*/

#include "astrolog.h"


/* Print a list of every key that one can press in a graphics window to do  */
/* a certain function, and a description of what it does, as displayed when */
/* one presses the 'H' or '?' key, and as shown with the -HX switch.        */

void DisplayKeysX()
{
	byte sz[cchSzDef];

	sprintf(sz, "Touches raccourcis pour les options graphiques d'%s (version %s):\n", szAppName, szVersionCore);
	PrintS(sz);
	PrintS(" Pressez 'H' ou '?' pour afficher cette liste des raccourcis.");
	PrintS(" Pressez 'p' pour faire une pause ou reprendre l'animation.");
	PrintS(" Pressez 'x' pour inverser la couleur du fond d'ecran.");
	PrintS(" Pressez 'm' pour basculer alternativement de la couleur au monochrome.");
	PrintS(" Pressez 'i' pour modifier l'affichage.");
	PrintS(" Pressez 't' pour afficher/effacer les infos en bas du theme.");
	PrintS(" Pressez 'b' pour afficher/effacer la bordure autour du theme.");
	PrintS(" Pressez 'l' pour afficher/effacer les glyphes des planetes.");
	PrintS(" Pressez 'j' pour nettoyer (ou pas) l'affichage a chaque modification.");
	PrintS(" Pressez 'v' pour basculer alternativement du graphisme au texte.");
	PrintS(" Pressez 'U' pour inverser les restrictions des etoiles fixes.");
	PrintS(" Pressez 'c' pour afficher le theme natal.");
	PrintS(" Pressez 'Alt'+'c' pour afficher les themes superposes.");
        PrintS(" Pressez 's', 'h', 'f', 'g', 'z', 'y' pour activer/desactiver le theme");
	PrintS("       sideral, heliocentrique, signes et maisons renverses, decans,");
	PrintS("       roue vedique et navamsa.");
	PrintS(" Pressez 'O' and 'o' pour rappeler/ranger le theme en memoire.");
	PrintS(" Pressez 'B' pour ajuster la carte a la fenetre.");
	PrintS(" Pressez 'Q' pour obtenir une carte bien ronde.");
	PrintS(" Pressez '<' ou '>' pour augmenter/diminuer la taille des glyphes");
	PrintS("       ou de la carte du monde.");
	PrintS(" Pressez '[' and ']' pour incliner le globe (clavier anglais).");
	PrintS(" Pressez '+' and '-' pour avancer/reculer d'un jour le theme.");
	PrintS(" Pressez 'n' pour calculer le theme de maintenant.");
	PrintS(" Pressez 'N' pour arreter/enclencher l'animation du theme et du globe");

	PrintS(" Pressez '!'-'(' pour mettre a jour le theme en ajoutant (clavier anglais):");
	PrintS("       !: secondes, @: minutes, #: heures, $: jours, %: mois,");
	PrintS("       ^: annees, &: annees*10, *: annees*100, (: annees*1000.");
	PrintS(" Pressez 'r' pour inverser la direction de l'animation.");
	PrintS(" Pressez '1'-'9' pour modifier la taille du Pas de l'animation de 'n' unites.");
	PrintS(" Pressez 'V','A','Z','S','M','K','J','L','E','W','G','P' pour afficher:");
	PrintS("       theme (_v), grille (_g), horizon (_Z), Syst. solaire (_S), secteur (_l),");
	PrintS("       calendrier (_K), maitrises (_j), astrocartographie (_L), ephemerides");
	PrintS("       (_E), carte du monde (_XW), globe (_XG), globe polaire (_XP).");
	PrintS(" Pressez 'Y' pour afficher le biorythme.");
	PrintS(" Pressez '0' pour passer de _Z a _Z0, de _XW a _XW0, de _E a _Ey.");

#ifdef CONSTEL
	PrintS(" Pressez 'F' pour afficher alternativement carte du monde et constellation.");
#endif

	PrintS(" Pressez 'espace' pour mettre a jour le graphisme.");
	PrintS(" Pressez 'efface arriere' pour effacer l'ecran.");
	PrintS(" Pressez 'page precedente','page suivante','debut','fin' pour monter");
	PrintS("       ou descendre l'ecran.");
	PrintS(" Pressez 'entree' pour saisir une ligne de commande.");
	PrintS(" Pressez 'q' pour quitter le logiciel.");
	PrintL();
	PrintS(" Bouton gauche de la souris: pour gribouiller l'ecran.");
	PrintS(" Bouton droit de la souris : pour saisir les coordonnes d'un lieu sur la carte du monde.");
}

/* Print a list of every command switch that can be passed to the program, */
/* and a description of what it does. This is what the -H switch prints.   */

void DisplaySwitches()
{
	byte sz[cchSzDef];

	sprintf(sz, "Commandes d'execution (version %s):\n", szVersionCore);
	PrintS(sz);
	PrintS(" _H: Affiche cette aide.");
	PrintS(" _HC: Affiche le nom des signes et des maisons.");
	PrintS(" _HO: Affiche les planetes et autres objets disponibles.");
	PrintS(" _HA: Affiche les aspects, leurs angles et orbes.");
#ifdef CONSTEL
	PrintS(" _HF: Affiche la liste des constellations.");
#endif
	PrintS(" _HS: Affiche les informations des planetes du systeme solaire.");
#ifdef INTERPRET
	PrintS(" _HI: Affiche la signification des signes, maisons, planetes et aspects.");
#endif
	sprintf(sz, " _He: Affiche toutes les infos ensembles (_Hc_H_Y_HX_HC_HO_HA%s_HS%s).",
#ifdef CONSTEL
			"_HF",
#else
			"",
#endif
#ifdef INTERPRET
			"_HI");
#else
			"");
#endif
	PrintS(sz);
	PrintS(" _Q: Permet d'autres commandes apres un affichage.");
	PrintS(" _Q0: Comme _Q mais au demarrage.");
	PrintS(" _M <1-100>: Execute la macro specifiee.");
	PrintS(" _M0 <1-100> <string>: Definit la macro specifiee.");
	PrintS(" _Y: Affiche la liste des commandes moins courantes.");
	PrintS("\nCommandes determinant le type de carte a afficher:\n");
	PrintS(" _v: Affiche le theme natal (choix par defaut).");
	PrintS(" _v0: comme _v mais avec vitesse relative au Soleil (invalide si Ephemerides exterieures actives).");
	PrintS(" _w [<lignes>]: Affiche le theme Mandala.");
	PrintS(" _w0 [..]: Comme _w mais renverse ordre des planetes des maisons 4..9.");
	PrintS(" _g: Affiche la grille des aspects et mi-points.");
	PrintS(" _g0: Comme _g avec les configurations planetaires en plus (Yod, etc) too.");
	PrintS(" _g0: Dans les synastries, montre les mi-points au lieu des aspects.");
	PrintS(" _ga: Comme _g mais indique les aspects appliquants au lieu des orbes.");
	PrintS(" _gp: Comme _g mais indique les aspects paralleles et contre-paralleles.");
	PrintS(" _a: Affiche la liste de tous les aspects classes par influence.");
	PrintS(" _a0: Comme _a mais affiche en plus la somme des aspects a la suite.");
	PrintS(" _a[0]a: Comme _a avec indication appliquant ou separatif.");
	PrintS(" _a[0]p: Comme _a mais indique les aspects paralleles et contre-paralleles.");
	PrintS(" _m: Affiche les mi-points classes dans l'ordre du zodiaque.");
	PrintS(" _m0: Comme _m mais affiche la somme des mi-points par signe en plus.");
	PrintS(" _ma: Comme _m avec les aspects des mi-points aux planetes en plus.");
	PrintS(" _Z: Affiche les positions des planetes par rapport a l'horizon local.");
	PrintS(" _Z0: Comme _Z mais affiche les coordonnees par rapport au centre polaire.");
	PrintS(" _Zd: Affiche les heures de levers, culminations et coucher des planetes.");
	PrintS(" _S: Affiche x,y,z coordonnees des planetes dans l'espace.");
	PrintS(" _l: Affiche les secteurs Gauquelin pour chaque planete.");
	PrintS(" _l0: Comme _l mais secteurs approximatifs utilisant les cuspides Placidus.");
	PrintS(" _j: Affiche les forces de chaque objet du theme.");
	PrintS(" _j0: Comme _j mais inclut les forces de chaque signe du zodiaque.");
	PrintS(" _L [<pas>]: Affiche l'astrocartographie.");
	PrintS(" _L0 [..]: Comme _L mais affiche aussi la liste des croisements de latitude.");
	PrintS(" _K: Affiche le calendrier mensuel.");
	PrintS(" _Ky: Comme _K mais affiche le calendrier annuel.");
	PrintS(" _d [<pas>]: Affiche les aspects, changements de signe ou de direction du jour.");
	PrintS(" _dm: Comme _d mais pour le mois.");
	PrintS(" _dy: Comme _d mais pour l'annee.");
	PrintS(" _dY <annees>: Comme _d mais pour un certain nombre d'annees.");
	PrintS(" _dp <mois> <annee>: Affiche les aspects du theme progresse pour le mois et l'annee.");
	PrintS(" _dpy <annee>: Comme _dp mais pour l'annee precisee.");
	PrintS(" _dpY <annee> <annees>: Comme _dp mais a partir de l'annee precisee, sur plusieurs annees.");
	PrintS(" _dp[y]n: Affiche les aspects progresses du mois (ou de l'annee).");
	PrintS(" _D: Comme _d mais classe les aspects par influence au lieu de l'heure.");
	PrintS(" _E: Affiche les ephemerides du mois.");
	PrintS(" _Ey: Affiche les ephemerides de l'annee.");
	PrintS(" _EY <annees>: Affiche les ephemerides pour plusieurs annees.");
	PrintS(" _e: Regroupe les commandes suivantes _v_w_g0_a_m_Z_S_l_K_j0_L0_d_D_E.");
	PrintS(" _t <mois> <annee>: Affiche les transits sur le natal pour le mois indique.");
	PrintS(" _tp <mois> <annee>: Affiche les progressions sur le natal pour le mois indique.");
	PrintS(" _tr <mois> <annee>: Affiche les retours planetaires pour le mois indique.");
	PrintS(" _t[p]y: <annee>: Affiche les transits/progressions pour l'annee indiquee.");
	PrintS(" _t[p]Y: <annee> <annees>: Affiche les transits a partir de l'annee indiquee sur plusieurs annees.");
	PrintS(" _t[py]n: Affiche les transits de maintenant au natal.");
	PrintS(" _T <mois> <jour> <annee>: Affiche les transits par ordre d'influence.");
	PrintS(" _Tp <mois> <jour> <annee>: Affiche les progressions au lieu des transits.");
	PrintS(" _T[p]n: Affiche les transits de maintenant par influence.");
#ifdef ARABIC
	PrintS(" _P [<parts>]: Affiche la liste des Parts Arabes avec leurs positions.");
	PrintS(" _P0 [<parts>]: Comme _P mais inverse les termes des formules des Parts.");
	PrintS(" _P[z,n,f]: Classe les Parts par position, nom, ou formule.");
#endif
#ifdef INTERPRET
	PrintS(" _I [<colonnes>]: Affiche l'interpretation du theme.");
#endif

	PrintS("\nCommandes affectant les parametres de calcul du theme:\n");
	PrintS(" _n: Calcule le theme de maintenant en utilisant l'heure de l'ordinateur.");
	PrintS(" _n[d,m,y]: Calcule le theme commencant le jour, le mois ou l'annee actuelle.");
	PrintS(" _z [<zone>]: Change le fuseau horaire par defaut (pour _d_E_t_q).");
	PrintS(" _z0 [<offset>]: Tient compte ou non de l'heure d'ete.");
	PrintS(" _zl <long> <lat>: Change les longitude et latitude par defaut.");
	PrintS(" _zt <heure>: Change seulement l'heure du theme.");
	PrintS(" _zd <jour>: Change seulement le jour du theme.");
	PrintS(" _zm <mois>: Change seulement le mois du theme.");
	PrintS(" _zy <annee>: Change seulement l'annee du theme.");
	PrintS(" _zi <nom> <ville>: Change seulement le nom et la ville du theme.");
	PrintS(" _q <mois> <jour> <annee> <heure>: Calcule le theme de la date indiquee avec les valeurs par defaut.");
	PrintS(" _qd <mois> <jour> <annee>: Calcule le theme de la date indiquee pour midi.");
	PrintS(" _qm <mois> <annee>: Calcule le theme pour le 1er jour du mois.");
	PrintS(" _qy <annee>: Calcule le theme pour le 1er jour de l'annee.");
	PrintS(" _qa <mois> <jour> <annee> <heure> <fuseau> <long> <lat>:");
	PrintS("     Calcule le theme avec les infos indiquees.");
	PrintS(" _qb <mois> <jour> <annee> <heure> <ete> <fuseau> <long> <lat>:");
	PrintS("     Comme _qa mais avec l'heure d'ete en plus.");
	PrintS(" _qj <jour>: Calcule le theme pour le jour julien specifie.");
	PrintS(" _i <fichier>: Calcule le theme d'apres les infos du fichier.");
	PrintS(" _i[2,3,4] <fichier>: Ouvre le fichier-theme en 2, 3 ou 4ieme position.");
	PrintS(" _o <fichier> [..]: Sauve les donnees d'entree du theme dans le fichier.");
	PrintS(" _o0 <fichier> [..]: Comme _o mais enregistre les positions des planetes et des maisons.");
	PrintS(" _os <fichier>, > <fichier>: Enregistre la sortie texte visible a l'ecran dans un fichier.");

	PrintS("\nCommandes affectant les informations utilisees dans le theme:\n");
	PrintS(" _R [<obj1> [<obj2> ..]]: Retire de l'affichage les planetes ou objets specifies.");
	PrintS(" _R0 [<obj1> ..]: Comme _R mais retire tous les objets.");
	PrintS(" _R1 [<obj1> ..]: Comme _R0 mais annule les restrictions et montre tous les objet.");
	PrintS(" _R[C,u,U]: Annule l'affichage des cuspides, des uraniennes ou des etoiles.");
	PrintS(" _RT[0,1,C,u,U] [..]: Restreint les planetes transitantes de la commande _t.");
	PrintS(" _RA [<asp1> ..]: Restreint les aspects.");
	PrintS(" _C: Affiche les 12 cuspides dans le theme (texte).");
	PrintS(" _u: Affiche les transneptuniennes/uraniennes dans le theme.");
	PrintS(" _U: Affiche les etoiles dans le theme.");
	PrintS(" _U[z,l,n,b]: Affiche les etoiles par ordre d'azimuth, d'altitude, de nom ou de brillance.");
	PrintS(" _Ao <aspect> <orb>: Specifie l'orbe maximum de l'aspect.");
	PrintS(" _Am <planet> <orb>: Specifie l'orbe maximum de la planete.");
	PrintS(" _Ad <planet> <orb>: Specifie l'orbe qui sera ajoute a celui de la planete.");
	PrintS(" _Aa <aspect> <angle>: Change la valeur habituelle de l'angle de l'aspect.");
	PrintS("\nCommandes affectant le calcul du theme:\n");
	PrintS(" _b: Utilise les fichiers d'ephemerides exterieures.");
	PrintS(" _b0: Comme _b mais affiche les positions a la seconde pres.");
	PrintS(" _c <valeur>: Selectionne une nouvelle domification.");
	PrintS("     (0 = Placidus, 1 = Koch, 2 = Egales, 3 = Campanus,");
	PrintS("     4 = Meridienne, 5 = Regiomontanus, 6 = Porphyre, 7 = Morin,");
	PrintS("     8 = Topocentrique, 9 = Alcabitius, 10 = Egales (MC),");
	PrintS("     11 = Neo-Porphyre, 12 = Entieres, 13 = Vediques, 14 = Aucune.)");
	PrintS(" _s [..]: Utilise le zodiaque sideral.");
	PrintS(" _sr: Calcule les positions en ascension droite sur l'equateur.");
	PrintS(" _s[z,h,d]: Affiche les positions dans le signe, en heures/minutes ou sur 360 degres.");
	PrintS(" _h [<objnum>]: Calcule les positions centrees sur l'objet precise.");
	PrintS(" _p <mois> <jour> <annee>: Calcule les progressions secondaires pour la date.");
	PrintS(" _p0 <mois> <jour> <annee>: Calcule le theme d'apres l'arc solaire pour cette date.");
	PrintS(" _p[0]n: Calcule le theme progresse (ou d'arc solaire) pour maintenant.");
	PrintS(" _pd <days>: Calcule un theme de direction pour le pas specifie (defaut 365.25).");
	PrintS(" _x <value>: Calcule un theme harmonique d'apres le facteur precise.");
	PrintS(" _1 [<objnum>]: Calcule le theme avec l'objet specifie a l'Ascendant.");
	PrintS(" _2 [<objnum>]: Calcule le theme avec l'objet specifie au Milieu du ciel.");
	PrintS(" _3: Affiche les positions en Decans.");
	PrintS(" _f: Inverse signe et maison pour les positions des planetes.");
	PrintS(" _G: Calcule les maisons d'apres la position geographique seulement (Geodesie).");
	PrintS(" _J: Affiche les maisons au format Vedique (sens inverse).");
	PrintS(" _9: Affiche les positions des planetes dans le zodiaque navamsa.");
	PrintS(" _F <objnum> <sign> <deg>: Force un objet a prendre la position indiquee.");
	PrintS(" _+ [<jours>]: Calcule le theme pour un certain nombre de jours dans le futur.");
	PrintS(" _- [<jours>]: Calcule le theme pour un certain nombre de jours dans le passe.");
	PrintS(" _+[m,y] [<valeur>]: Calcule le theme pour un certain nombre de mois/annees dans le futur.");
	PrintS("\nCommandes pour les synastries et themes compares:\n");
	PrintS(" _r <fichier1> <fichier2>: Affiche la synastrie des 2 themes (planetes theme2 dans les maisons de theme1).");
	PrintS(" _rc <fichier1> <fichier2>: Affiche le theme composite (mi-points des planetes et cuspides de 1 et 2.");
	PrintS(" _rm <fichier1> <fichier2>: Affiche le theme mi-espace mi-temps (pour un lieu et une date medians.");
	PrintS(" _r[c,m]0 <fichier1> <fichier2> <ratio1> <ratio2>: Theme synastrie (ou composite,mi-espace/temps) ponderable.");
	PrintS(" _rd <fichier1> <fichier2>: Affiche le temps ecoule entre les 2 dates des themes.");
#ifdef BIORHYTHM
	PrintS(" _rb <fichier1> <fichier2>: Affiche le biorythme du fichier1 au temps du fichier2.");
#endif
	PrintS(" _r0 <fichier1> <fichier2>: Affiche la synastrie des 2 themes superposes (theme2 sur theme1).");
	PrintS(" _rp[0] <fichier1> <fichier2>: Comme _r0 mais fait progresser le theme1 a la date du 2.");
	PrintS(" _rt <fichier1> <fichier2>: Comme _r0 mais traite fichier2 comme un transit sur le 1.");
	PrintS(" _r[3,4]: Superpose 3 ou 4 themes.");
	PrintS(" _y <fichier>: Affiche les transits en maison pour le fichier.");
#ifdef BIORHYTHM
	PrintS(" _y[b,d,p,t] <fichier>: Comme _r0 mais compare avec l'heure courante (maintenant).");
#endif
	PrintS("\nCommandes d'options graphiques:\n");
	PrintS(" _k: Affiche les themes au format texte/couleur (caracteres ANSI).");
	PrintS(" _k0: Comme _k mais sans les couleurs.");

	PrintS(" _X: Affiche la representation graphique (au lieu du texte).");
	PrintS(" _Xb: Cree un fichier bitmap au lieu d'un affichage ecran.");
	PrintS(" _Xb[n,c,v,a,b]: Cree un fichier bitmap en mode X11 normal,");
	PrintS("     compacte, tres compact, Ascii (bmtoa) ou Windows bmp.");
#ifdef PS
	PrintS(" _Xp: Cree un fichier PostScript au lieu de bitmap.");
	PrintS(" _Xp0: Comme _Xp mais cree un fichier complet au lieu d'encapsule.");
#endif
#ifdef META
	PrintS(" _XM[0]: Cree un fichier Windows metafile (wmf) au lieu de bitmap.");
#endif
	PrintS(" _Xo <fichier>: Enregistre le graphique dans le fichier.");
	PrintS(" _Xm: Affiche la carte du theme en monochrome.");
	PrintS(" _Xr: Affiche la carte du theme en renversant la couleur du fond.");
	PrintS(" _Xw <hor> [<ver>]: Change la taille du theme graphique.");
	PrintS(" _Xs <100,200,300,400>: Change la taille des cartes ou des caracteres en %.");
	PrintS(" _Xi: Modifie l'affichage du theme (different selon les fonctions activees).");
	PrintS(" _Xt: Annule l'affichage d'infos sur le cote du theme.");
	PrintS(" _Xu: Annule l'affichage de la bordure du theme.");
	PrintS(" _Xl: Annule l'affichage du glyphe des planetes.");
	PrintS(" _Xj: Ne redessine pas l'ecran entre les mises a jour des cartes.");
	PrintS(" _X1 <objet>: Tourne la carte de maniere a avoir l'objet a gauche.");
	PrintS(" _X2 <objet>: Tourne la carte de maniere a avoir l'objet en haut.");
	PrintS(" _XW: Affiche la carte du monde.");
	PrintS(" _XW0: Comme _XW mais au format ovale.");
	PrintS(" _XG [<degres>]: Affiche le globe terrestre.");
	PrintS(" _XP: Comme _XG mais en projection polaire.");
#ifdef CONSTEL
	PrintS(" _XF: Affiche les constellations sur la sphere celeste.");
#endif
	PrintS(" _Xn [<mode>]: Lance l'animation du theme ou du globe.");
	PrintS(" _HX: Affiche la liste des raccourcis clavier.");
	
	PrintS(" _W <value>: Lance la version Windows avec une commande interne.");
	PrintS(" _WN <1-32000>: Regle le delai de mise a jour des animations en millisecondes.");
	PrintS(" _WM <1-100> <texte>: Cree une macro-commande a partir du texte.");
	PrintS(" _Wn: Ne redessine pas l'ecran.");
}


/* Print a list of the obscure command switches that can be passed to the */
/* program and a description of them. This is what the -Y switch prints.  */

void DisplaySwitchesRare()
{
	byte sz[cchSzDef];

	sprintf(sz, "Autres commandes (version %s):\n", szVersionCore);
	PrintS(sz);
	PrintS(" _Y: Affiche cette aide.");
	PrintS(" _Yn: Affiche les Noeuds Lunaires Vrais au lieu de Moyens.");
	PrintS(" _Yd: Affiche les dates au format J/M/A au lieu de M/J/A.");
	PrintS(" _Yt: Affiche les heures sur 24 heures au lieu de 12 heures (am/pm).");
	PrintS(" _YC: Ignore les aspects aux maisons succedentes et cadentes.");
	PrintS(" _Y8: Limite et coupe l'affichage des textes a 80 colonnes.");
	PrintS(" _YQ <lignes>: Insere une pause dans le defilement des pages d'ecran apres le nombre de lignes indique.");
	PrintS(" _Yo: Affiche les donnees et positions du theme au format de l'ancienne version.");
	PrintS(" _Yc: Cuspides des angles alignees sur leurs positions.");
	PrintS(" _Yl <1-36>: Indique si un secteur Gauquelin est un secteur + ou non.");
#ifdef ARABIC
	PrintS(" _YP <-1,0,1>: Modifie la maniere de calculer les Parts Arabes en nocturne.");
#endif
	PrintS(" _Yb <jours>: Precise le nombre de jours couverts par les biorythmes.");
	PrintS(" _YE <objet> <axe semi-majeur> <excentricite (3 parametres)> <inclinaison (3 p.)>");
	PrintS("     <perihelion (3 p.)> <noeud ascendant (3 p.)> <chronometre le decalage (3 p.)>");
	PrintS("     Change l'orbite de l'object pour les elements donnes.");
	PrintS(" _YR <obj1> <obj2> <drapeau1>..<drapeau2>: Fixe les restrictions pour les objets.");
	PrintS(" _YRT <obj1> <obj2> <drapeau1>..<drapeau2>: Fixe les restrictions pour les transits.");
	PrintS(" _YR0 <drapeau1> <drapeau2>: Fixe les restrictions pour les changements de signe et de direction.");
	PrintS(" _YRZ <lever> <zenith> <coucher> <nadir>: Fixe les restrictions pour les heures de _Zd (0=affichee ou 1=exclue).");
	PrintS(" _YAR <asp1> <asp2> <flag1>..<flag2>: Fixe les restrictions pour les aspects.");
	PrintS(" _YAo <asp1> <asp2> <orb1>..<orb2>: Fixe les orbes des aspects specifies.");
	PrintS(" _YAm <obj1> <obj2> <orb1>..<orb2>: Fixe l'orbe alloue aux objets/planetes specifies.");
	PrintS(" _YAd <obj1> <obj2> <orb1>..<orb2>: Fixe l'orbe additionnel alloue aux objets/planetes specifies.");
	PrintS(" _YAa <asp1> <asp2> <ang1>..<ang2>: Fixe l'angle attribue aux aspects specifies.");
	PrintS(" _Yj <obj1> <obj2> <inf1>..<inf2>: Fixe l'influence attribuee aux objets specifies (agit sur _j, _a et _T).");
	PrintS(" _YjC <cusp1> <cusp2> <inf1>..<inf2>: Fixe l'influence attribuee aux cuspides specifiees (agit sur _j).");
	PrintS(" _YjA <asp1> <asp2> <inf1>..<inf2>: Fixe l'infuence des aspects specifies (agit sur _j, _a, _T et _D).");
	PrintS(" _YjT <obj1> <obj2> <inf1>..<inf2>: Fixe l'influence du transit des objets/planetes specifies (agit sur _T et _D).");
	PrintS(" _Yj0 <inf1> <inf2> <inf3> <inf4>: Fixe l'influence des dignites");
	PrintS("     dans l'ordre suivant : presence en signe de domicile, d'exaltation,");
    PrintS("     en maison de domicile, et d'exaltation (agit sur _j).");
	PrintS(" _YJ <obj> <sign> <cosign>: Fixe les domiciles d'une planete (0= aucun).");
	PrintS(" _YJ0 <obj> <sign>: Fixe l'exaltation d'une planete.");
	PrintS(" _YI <obj> <chaine de caracteres>: Personnalise le texte d'interpretation specifiant l'objet/planete.");
	PrintS(" _YIa <sign> <chaine de caracteres>: Personnalise les adjectifs qualifiant le signe.");
	PrintS(" _YIv <sign> <chaine de caracteres>: Personnalise les verbes decrivant le signe.");
	PrintS(" _YIC <maison> <chaine de caracteres>: Personnalise le texte d'interpretation de la maison.");
	PrintS(" _YIA <asp> <chaine de caracteres>: Personnalise le texte d'interpretation de l'aspect.");
	PrintS(" _YIA0 <asp> <chaine de caracteres>: Personnalise l'etat de l'interpretation de l'aspect.");
	PrintS(" _YkC <feu> <terre> <air> <eau>: Personnalise les couleurs des elements.");
	PrintS(" _YkA <asp1> <asp2> <coul1>..<coul2>: Personnalise les couleurs des aspects.");
	PrintS(" _Yk0 <1..7> <1..7> <coul1>..<coul2>: Change les couleurs 'arc-en-ciel'.");
	PrintS(" _Yk <0..8> <0..8> <col1>..<col2>: Change les couleurs 'generales'.");
	PrintS(" _YXC <0 (aucune) 1 (moire) 2 (toile)> <taille> <nb. lignes>: Decoration coins carte");
	PrintS(" _YXG <0-2><0-2><0-2><0-3>: Modifie les symboles");
	PrintS("     pour le Capricorne, Uranus, Pluton et Lilith.");
	PrintS(" _YXg <cellules>: Definit la taille des grilles (cellules x cellules).");
	PrintS(" _YXf <val>: Indique s'il faut utiliser les polices de caracteres habituelles.");
	PrintS(" _YXp <-1,0,1>: Fixe l'orientation du papier pour les fichiers PostScript.");
	PrintS(" _YXp0 <hor> <ver>: Fixe la taille du papier pour les fichiers PostScript.");
	PrintS(" _0[o,i,q,X]: Desactive les fichiers en sortie, en entree, l'arret du logiciel et les graphiques.");
	PrintS(" _;: Ignore la ligne de commande et la traite comme un commentaire.");
}

/* etc_fr.c */
