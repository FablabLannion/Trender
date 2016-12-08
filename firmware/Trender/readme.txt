
# Cahier des charges

* interface web simple et relativement jolie
* fonctions limitées
* un bouton/tilt pour le controle (start, stop)
* wifi toujours allumé pour accès à la configuration
* ssid et @ip gravées sous le boitier

## propositions pour le soft Trender

le code se base sur https://github.com/FablabLannion/Trender/tree/sweettime/firmware/Demo_neo

pages :
* une page configuration pour le temps total
* une page network info pour le fun :)
* (une page caché pour mettre la couleur que l'on veut)

Par défaut :
* 3 couleurs : vert, jaune rouge
* passage doux entre chaque couleur pendant toute la durée prévue entre 2 couleurs (ou juste sur 10% du temps restant avant le prochain seuil ?)
* vert->jaune : de 0 à 50% du temps
* jaune->rouge : de 50% à 95%
* rouge pulsant lent de 95% à 100%

Page configuration :
* entrer un temps libre
* choisir les seuils et couleurs
* sauvegarde du dernier profil choisi/couleurs en eeprom
* au boot : n flashs bleus pour indiquer le profil courant quand on est pret
* un tilt pour lancer le chrono
* un nouveau tilt pour passer en repos

## nice to have
* réglage des seuils
* pause sur le timer

