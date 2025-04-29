# 2024-2025 : Projet de capteur graphène low-tech – Sombret Yanis, Rolland Tristan

## SOMMAIRE

- [2024-2025 : Projet de capteur graphène low-tech – Sombret Yanis, Rolland Tristan](#2024-2025--projet-de-capteur-graphène-low-tech--sombret-yanis-rolland-tristan)
  - [SOMMAIRE](#sommaire)
  - [Présentation du projet](#présentation-du-projet)
  - [Livrables](#livrables)
  - [Matériel nécessaire](#matériel-nécessaire)
  - [Simulation électronique du capteur sous LTSpice](#simulation-électronique-du-capteur-sous-ltspice)
  - [Design du PCB sous KiCad et réalisation de la Shield](#design-du-pcb-sous-kicad-et-réalisation-de-la-shield)
  - [Code Arduino et applications](#code-arduino-et-applications)
  - [Test et résultats](#test-et-résultats)
  - [Datasheet](#datasheet)
  - [Conclusions et analyse critique du projet](#conclusions-et-analyse-critique-du-projet)
  - [Contacts](#contacts)

## Présentation du projet

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dans le cadre de l’UF «Du capteur au banc de test», nous avons eu pour projet de designer et produire un système de mesure de déformation d’une jauge de contrainte low-tech, un morceau de papier sur lequel est dessiné une piste au crayon à papier. Cette technologie bas coût de jauge de contrainte provient des travaux de Cheng-Wei Lin, Zhibo Zhao, Jaemyung Kim et Jiaxing Huang, chercheurs du département des sciences de l’ingénierie et des matériaux de l’université Northwestern à Evanston, USA. En effet, les traces de crayons laissées sur le papier jouent le rôle de fines couches de graphène conductrices. 

<p align="center">
  <img src="/Photos/piste-graphène.PNG" alt="piste-graphène">
</p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;En fonction de la contrainte appliquée à notre jauge, l’écartement des particules de graphène se voit modifié, altérant les propriétés conductrices de notre couche de graphène. Grossièrement, effectuer une flexion sur notre capteur revient à écarter les agglomérats de graphène diminuant la conductivité, effectuer une compression revient à produire l’effet inverse en rapprochant les agglomérats de graphène. 

<p align="center">
  <img src="/Photos/tension-compression.PNG" alt="tension-compression">
</p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Par mesure de tension en sortie d’amplificateur, il est possible de remonter à la valeur de la résistance du capteur et la mettre en regard de la déformation. Enfin, la quantité de graphène dépend du type de crayon employé influant aussi la conductivité de la couche obtenue. 

<p align="center">
  <img src="/Photos/crayons.PNG" alt="crayons">
</p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Ainsi, en plus de l’intérêt pédagogique de ce projet de conception qui nous a permis d’intervenir sur toute la chaîne d’acquisition, nous avons eu l’occasion, et pour mission, de faire une étude qualitative du banc de test et déterminer ainsi si le capteur pouvait faire l’objet d’une commercialisation, et si oui, avec quel type de crayon à papier. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dans le cadre de la réalisation et de la caractérisation de ce dispositif de mesure low-tech, il nous a fallu suivre différentes étapes allant de la simulation électronique à la rédaction de sa datasheet en passant par l’impression de notre Shield PCB sur lequel nous avons fait nos soudures.

Dans ce README, nous faisons état de tout ce qui a été fait dans le cadre de ce projet.

## Livrables

Plusieurs livrables sont attendus :

- **Un Shield PCB connecté à une carte Arduino UNO** avec différents composants : un capteur graphite, un amplificateur transimpédance, un module Bluetooth, un écran OLED, un flexsensor commercial, un potentiomètre digital, un encodeur rotatoire ;
- **Le code Arduino lié** permettant de contrôler les différents composants intégrés et de faire fonctionner les différentes fonctionnalités de notre assemblage (mesures de contraintes, échanges Bluetooth et OLED, potentiomètre digital et boutons) ;
- **Une application Android** (sous MIT App Inventor) interfaçant le PCB et le code Arduino correspondant, aidant à la prise des mesures ;
- **La datasheet du capteur** reprenant toutes les caractéristiques notre capteur low-tech.

## Matériel nécessaire

Afin de mener notre projet à son aboutissement, nous avons eu besoin de :

*Pour le montage de l'amplificateur transimpédance : *

- Résistances fixes : 1 kΩ x1, 10 kΩ x1, 100 kΩ x2 ; une résistance variable (le potentiomètre digital), afin de pouvoir régler le gain de notre amplificateur ;
- Une résistance variable (Potentiomètre digital MCP41050 et son support), afin de pouvoir régler le gain de notre montage amplificateur ;
- Amplificateur opérationnel LTC1050 et son support ;
- Capacités : 100 nF x3, 1 μF x1.

*Pour le reste de notre Shield :*

- Arduino Uno et son câble d’alimentation, notre carte d’acquisition ;
- Résistance : une de 33kΩ similaire à la résistance au repos du flexsensor afin de réaliser un pont diviseur de tension ;
- Module Bluetooth HC05 ;
- Ecran OLED de dimension 128*64 ;
- Flexsensor ;
- Encodeur rotatoire ;
- Capteur graphite et 2 pinces crocodiles ;

## Simulation électronique du capteur sous LTSpice

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Notre capteur passif en graphène possède une très grande résistance, de l’ordre du mégaohm. Ainsi, une fois alimenté par la carte Arduino en 5V à ces bornes, le courant est très faible. Or, il est notre signal d’intérêt pour remonter à la valeur de la résistance de notre capteur. Ainsi, il est nécessaire de mettre en place un amplificateur transimpédance afin d’amplifier le signal, le convertir en tension, et enfin de diminuer les bruits en entrés qui pourrait parasiter notre signal trop faible. Pour se faire, nous avons d’abord modélisé l’amplificateur sous LTspice afin d’optimiser au mieux sa conception pour notre application.

***Description du montage amplificateur transimpédance***

Nous avons testé ce [montage](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/blob/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/LTspice/Ampli_transimp%C3%A9dance/Ampli_transimp%C3%A9dance.asc) sur le logiciel LTspice :

![Montage-ampli](/Photos/Montage-ampli.PNG)

Via ce montage, on a le rapport :

Vcc/Vadc=(Rcapteur+R1+R5)/(1+R3/R2)R1

On peut donc remonter à la valeur de la résistance du capteur :

Rcapteur=(1+R3/R2)R1*Vcc/Vadc-R1-R5

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Aussi, nous remplacerons R2 sur notre PCB finale par un potentiomètre digital afin de pouvoir régler pendant nos mesures le gain du montage dans le cas de saturation par exemple.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Le choix de l’amplificateur opérationnel de notre circuit ici est crucial. Celui-ci doit être capable d'accepter en entrée un très faible courant et posséder un offset de tension très faible afin de ne pas fausser les valeurs de tension en sortie du montage. Contraintes vérifiées par simulation. Après plusieurs essaies guidés, le choix le plus pertinent était l’AOP LTC 1050.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Place maintenant à la réduction du bruit. Pour obtenir un signal le plus pur et ce, de manière efficace, nous avons dimensionné les 3 filtres de ce montage amplificateur transimpédance via nos simulations. Chaque filtre ayant un rôle bien précis : 

- à l'entrée, un filtre 1 passe-bas passif (R5,C1) avec une fréquence de coupure de 16 Hz permettant de filtrer les bruits sur le signal en entrée fournis par nos capteurs. Donc tout type de perturbations en courant qu’il pourrait avoir lieu avant amplification, qui serait amplifié dans le cas contraire avec notre signal, le noyant.

- un deuxième filtre 2 passe-bas avec une fréquence de coupure de 1.6 Hz (R3,C4) couplé à l'AOP permettant de filtrer la composante du bruit à 50 Hz provenant du réseau électrique. Ici, la simulation nous a permis de fixer la valeur de C4. Une valeur trop élevée permet de diminuer grandement le bruit à 50 Hz mais diminue fortement le temps de réponse de notre circuit amplificateur. Ainsi, après simulation, fixer C4 à 1μF représente le meilleur compromis. 

- à la sortie de l'amplificateur, le filtre 3 (R6,C2) avec une fréquence de coupure de 1.6 kHz permettant de filtrer le bruit provenant du fonctionnement du LTC1050 (bruits des alimentation, de l'horloge, etc...).

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Enfin, la résistance R5 en entrée, en plus de former avec la capacité C1 un filtre éliminant les bruits en tension, protège l'AOP contre les décharges électrostatiques et permet d’obtenir la conversion du signal en tension. La résistance R1 sert de résistance de shunt entre la source de courant et la masse.

***Résultats simulations***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Comme énoncé précédemment, la simulation nous a permis de vérifier la théorie et d’optimiser certains filtres afin d’obtenir les meilleurs rapports signal/temps de réponse. Voici les résultats des simulations.

![C4](/Photos/C4.PNG)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;On observe bien ci-dessus l’amplification du signal. L'Arduino pourra donc l'interpréter. De plus, on vérifie que plus C4 est élevée, meilleur le signal est mais plus le temps de réponse est long.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Ci-dessous les résultats de nos simulations à courant alternatifs afin de vérifier et optimiser le rôle de chacun des filtres. Le bruit est atténué d’environ 72dB à 50Hz. Le montage est performant.

![Filtre](/Photos/Filtre.PNG)

## Design du PCB sous KiCad et réalisation de la Shield

***Modélisation sous KiCad***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Une fois les étapes de vérification faites, nous pouvions commencer le design et la conception de notre banc de test. Pour se faire nous avons utilisé le logiciel de modélisation KiCad 9.0, qui permet de modéliser tous les composants dont nous avions besoin sur une Shield de taille identique à la plaquette Arduino qui servira de carte d’acquisition. De là, nous pouvions modéliser nos différents composant et faire la connectique, étape très importante étant donné la petitesse de notre PCB en regard des composants qu’elle accueillerait. Afin de réaliser nos mesures avec le plus de pertinence et de confort, nous avons eu recourt à:

- un flexsensor commercial d’une résistance au repos de 33kΩ, afin de pouvoir comparer nos mesures avec celle des capteurs graphites.

- un module bluetooth HC-05 permettant d’envoyer les données mesurées sur notre téléphone via une application Android codé par nos soins sur MIT APP Inventor.

- un écran OLED permettant d’afficher un menu sur lequel nous pouvons régler la valeur de la résistance du potentiomètre digital ainsi qu’observer la valeur des résistances du flexsensor et des capteurs graphènes. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pour la modélisation, nous avons commencé par faire la schématique. Cette dernière permet de visualiser et de faire tous les branchements nécessaires au bon fonctionnement de notre banc de test. Fi de fils de connections, c’est le fonctionnement qui importe. Nous avons dû créer nous même certains symboles non présents dans les bibliothèques déjà existantes. Notre seule contrainte ici était de connecter les bons éléments aux pins d’intérêts de l’arduino. En effet, certain composant nécessitait d’être connecté à des pins PWM de l’arduino pour cadencer l’information via une clock. C’est le cas pour le module HC-05 en écriture au moins et l’encodeur digital. Voici donc notre proposition :

![schematique](/Photos/schematique.PNG)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Par la suite, nous avons créé les empreintes de chacun des composants afin de modéliser notre Shield PCB. C’est à cette étape que nous prenions en compte les pistes de cuivres et l’agencement des composants pour que tout rentre sur l’espace imposé et que tout soit connecté selon la schématique que nous avions produit précédemment. Aussi, il nous a fallu prêter grande attention aux masses flottantes et aux piscines. Les masses flottantes pouvaient générer des disfonctionnements sur notre banc. Quant aux piscines, de larges zones sans cuivre, elles posaient un problème de gaspillage, les pistes après modélisation se faisant par enlèvement de matière.  

<p align="center">
  <img src="/Photos/PCB.PNG" alt="PCB">
</p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Il nous a également fallu prendre en compte les dimensions de chaque composant pour éviter qu’ils ne se chevauchent au moment de la réalisation :

![3D](/Photos/3D.PNG)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Les fichiers KiCad sont disponibles dans le dossier [KiCad arduino uno](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/tree/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/Kicad%20arduino%20uno/Capteur_graphite).

***Réalisation de la Shield***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Modélisation terminée, nous avons pu passer à la réalisation physique de notre Shield. Voici les étapes de fabrication réalisées :

Étape 0 (importante !) : vérification du PCB sous KiCad ;
Étape 1 : édition du masque de gravure de notre PCB sous KiCad ;
Étape 2 : insolation UV d'une plaquette d'époxy recouverte d'une couche de cuivre et de résine photosensible ;
Étape 3 : immersion de la plaquette dans un révélateur afin d'éliminer la résine non-insolée ;
Étape 4 : immersion de la plaquette dans du perchlorure de fer pour graver les pistes ;
Étape 5 : nettoyage de la plaquette avec de l'acétone pour retirer la résine restante.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Toutes ces étapes de fabrication ont été réalisées par Cathy Crouzet. Nous avons ensuite pu réaliser le perçage de notre plaquette et y souder nos divers composants selon le schéma du PCB obtenu sur KiCad.

<p align="center">
  <img src="/Photos/Soudure.PNG" alt="Soudure">
</p>

Voici le résultat après soudure de notre PCB :

![Résultat_final1](/Photos/Résultat_final1.jpg)

![Résultat_final2](/Photos/Résultat_final2.jpg)

## Code Arduino et applications

***Arduino***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Une fois le Shield PCB réalisé, nous avons d’abord réalisé le code Arduino qui nous permettrait de coordonner tous nos composants et procéder nos mesures.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Nous avons décidé de donner une importance particulière à l’écran OLED, notre but étant de configurer un petit menu pour y sélectionner les valeurs de résistance du potentiomètre et pour y voir les valeurs de résistance mesurées. Le fonctionnement de l’affichage OLED se fait en parallèle de la mesure, à chaque tour de boucle, que l’on soit ou non sur la partie du menu nous permettant de voir la mesure, un signal est récupéré. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Le deuxième élément qui nous a demandé beaucoup d’attention était le module Bluetooth. Nous avons opté pour un fonctionnement simple lié au menu d’affichage de l’écran OLED. Si la mesure se fait de manière constante, l’envoie de données lui ne l’est pas. Si nous nous trouvons sur la page du menu de configuration de la valeur du potentiomètre, l’information envoyée est un entier compris entre 1 et 8 codé sur 8 bits. Si nous nous trouvons sur la page du menu affichant les valeurs de résistance, l’information envoyée est un entier compris entre 0 et 255 codé sur 8 bits également.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Ici n’est qu’un bref résumé du fonctionnement de notre code. Notre [code](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/blob/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/Kicad%20arduino%20uno/partie_software/code_arduino/code_arduino.ino) complet et commenté se trouve sur le GitHub.

***Application Android***

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Enfin, avant de pouvoir commencer à mesurer, nous avons procédé à la création d’une application Android sur MIT APP Inventor afin de mieux visualiser les données. Le fonctionnement de notre code block est directement lié à notre code Arduino du HC-05. En fonction de la donnée reçue, nous complétons le champ « valeur résistance SPI » ou « valeur résistance mesurée ».

<p align="center">
  <img src="/Photos/App-inventor.PNG" alt="App-inventor">
</p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Nous avons fait face à de nombreux problème lors de sa réalisation, le plus gros étant le cadencement des données. En effet, notre HC-05 envoyait pléthore de données et saturait le buffer de réception. Ainsi nous pouvions arrêter les mesures à un temps T et n’arrêter d’en afficher les variations sur l’application qu’une à deux minutes plus tard. Nous avons résolu ce problème en réglant la clock de l’application pour la caler sur le bauderate de l’Arduino. Une fois fait, nous avons tenté de faire un graphique en temps réel pour visualiser les variations mais celui-ci n’a pas abouti, les variations étant trop rapides et trop grandes. 

Idem, l’[application](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/blob/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/Kicad%20arduino%20uno/Appli_Apk/Capteur.apk) est disponible sur le GitHub.

## Test et résultats

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Afin de réaliser nos tests et mesures avec la plus grande précision et confort possible, nous voulions nous assurer des contraintes que nous appliquions sur nos jauges. Nos jauges car nous avons confectionné plusieurs jauges de contrainte avec des types de crayon différents dans l’optique de pouvoir les comparer les unes aux autres et déterminer laquelle serait la plus performante ou la plus conductrice. Celles-ci sont toutes de dimensions fixes avec une épaisseur de papier fixe.

<p align="center">
  <img src="/Photos/dimension-capteur.PNG" alt="dimension-capteur">
</p>

Or, la déformation ε dans notre cas se calcule comme :  

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ε=e/(2*R0)

Où e est l’épaisseur de nos jauges, et R0 le rayon de courbure. Ainsi, pour pouvoir appliquer la même contrainte et donc observer la même déformation sur toutes nos jauges de contrainte, nous avons modélisé et fabriqué le petit outil de test ci-dessus.

<p align="center">
  <img src="/Photos/bancdetest.JPG" alt="bancdetest">
</p>

Il dispose de 6 encoches dans lesquelles nous pouvons glisser nos jauges en papiers. Les 6 encoches sont de dimensions fixes et donc, applique une contrainte similaire sur chacune des jauges à tester. 

![dimension](/Photos/dimension.JPG)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;De fait, avec des conditions de mesure similaire, nous avons pu comparer les différents capteurs les uns aux autres et produire le tableau de [résultats](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/blob/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/R%C3%A9sultats/Mesures.xlsx) disponible sur notre GitHub. Cependant, comme développé en conclusion nous avons rencontré quelques soucis durant nos mesures amenant à des quelques aberrations. Par exemple ci-dessous on peut voir sur le graphique avec la valeur du potentiomètre fixé à 12500 Ω que le ∆R/R est plus marqué pour le 2B et le B que pour le 3B. Or, le 3B est censé être plus gras que le B et le 2B, il devrait donc avec des ∆R/R plus élevé pour une même déformation.

<p align="center">
  <img src="/Photos/deltaR_12500.PNG" alt="deltaR_12500">
</p>

Aussi, ci-dessous nous pouvons voir les valeurs de résistances en fonction de la déformation pour différents crayons, potentiomètre fixé à 12500 Ω. Sur ce graphique, on constate que la résistance du crayon HB ne correspond pas à la théorie, moins gras, sa résistance devrait être grandement supérieur aux crayons de type B et plus.

<p align="center">
  <img src="/Photos/R_12500_flexion.PNG" alt="R_12500_flexion">
</p>

On peut expliquer cette constatation par le fait que chaque jauge a été dessiné en appuyant la mine des crayons de manière plus ou moins forte sur le papier, déposant des quantités de graphènes différentes. Ainsi, ici, on peut supposer que le capteur HB a été dessiné avec violence, ce qui a fortement diminué sa résistance dû à la forte concentration de graphène malgré le crayon peu gras qu’est le HB.

## Datasheet

La datasheet de notre capteur est disponible [ici](https://github.com/MOSH-Insa-Toulouse/2024-2025-4GP-Glubulax-TTRollanDD/blob/5d5dbacb22e8d05c2202cd88c66fef8cc0433a9e/Datasheet.pdf).

## Conclusions et analyse critique du projet

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Ce projet a été très enrichissant d’un point de vue pédagogique. Ce fut l’occasion pour nous développer plusieurs compétences de savoir-faire. Des compétences de savoir-faire car via ce projet nous avons pu intervenir et manipuler à tous les niveaux de la chaîne d’acquisition, faisant le lien entre des phénomènes physiques et le monde du numérique beaucoup plus pratique pour l’analyse. A ça, nous avons pu développer des compétences en électroniques très importantes, notamment au niveau de la réduction de bruit pour ne garder que le signal d’intérêt, parfois très faible, mais garder un temps de réponse raisonnable. Enfin, à travers ce projet nous avons obtenu une première approche de confection de datasheet, très importante bien que peu attirante, pour faciliter la lecture de nos travaux.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;En ce qui concerne les capteurs graphites fabriqués, ils sont fonctionnels et les tests nous permettent de dégager des tendances cohérentes avec la physique. Cependant, nos capteurs présentaient des valeurs relativement variables. D’un jour à l’autre nous n’obtenions pas tout à fait les mêmes résultats pour une même jauge, bien que nous ayons fixé les contraintes appliquées via notre outil de mesure. De plus, notre outil du fait des encoches, gratte la fine couche de graphène et en enlève, altérant les propriétés des capteurs. Aussi, on observait des oscillations constantes dans nos mesures mettant en évidence, malgré nos filtres, un bruit externe venant les perturber. A ça nous aurions pu, par exemple, pour améliorer notre dispositif blinder les câbles, mais aussi notre Shield. De fait, il est nécessaire de prendre du recul sur les résultats que nous proposons.

## Contacts

Pour toute information complémentaire, vous pouvez nous contacter à ces adresses mails :

- Sombret Yanis : <sombret@insa-toulouse.fr>
- Rolland Tristan : <rollan@insa-toulouse.fr>


