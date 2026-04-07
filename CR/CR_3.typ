#let header(title, color: red) = {
  set text(black, 30pt)
  set align(center+top)
  rect(
    fill: color,
    inset: 10pt,
    rect(
      fill: white,
      stroke: color,
      inset: 35pt,
      width: 100%,
      [#title],
    )
  )
}

#let exoTitre(number) = {
  set text(black, 20pt)
  rect(
    fill: aqua,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [#"Exercice "#number],
  )
}

#let exEnonce(body) = {
  set text(15pt)
  rect(
    fill: silver,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [#body],
  )
}

#let exo(number, body) = {
  stack(
    dir: ttb,
    exoTitre(number),
    exEnonce(body),
  )
}

#let rectangle(body, fill: silver, stroke: none) = {
  set text(black)
  set align(center)
  rect(
    fill: fill,
    stroke: stroke,
    inset: 2pt,
    radius: 4pt,
    [#body],
  )
}

#let solution(body) = {
  set text(black)
  set align(center)
  rect(
    fill: lime,
    inset: 20pt,
    radius: 4pt,
    [#body],
  )
}

#set page(
  margin: 5pt
)
#set text(15pt)
#header([Projet image : Mosaïque], color: gray)

#rect(
    fill: aqua,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [Tommy-Verdi ORAVEC, Lucas PAULO, Victor ONIC],
  )

= Améliorations apportées à la mosaïque d'images

== Ajout d'une fonction de découpage d'image selon un masque
Nous avons implémenté une fonction de découpage d'image selon un masque binaire. Cette fonction prend en entrée une image et un masque, et retourne une nouvelle image contenant uniquement les pixels de l'image d'origine correspondant aux pixels blancs du masque. Cette fonction sera utile notament pour l'implémentation du superpixel.
\

#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto, auto, auto),
      rows: (auto, 1em),
      row-gutter: (1em),
      image("wpuech.jpg", width: 90%),
      image("mask.png", width: 88%),
      image("cut.png", width: 88%),
      [Image originale],
      [Masque binaire],
      [Image découpée selon le masque],
    )
 ]
], fill: silver, stroke: black)
)
\

= Jigsaw mosaïque via superpixel

Nous avonc ajouté une mosaïque d'image dont la grille est remplacé par des superpixel afin de modifier
les imagettes placées via un masque pour qu'elles épousent mieux les contours dans l'image.

#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto),
      rows: (auto, 1em),
      row-gutter: (1em),
      image("SnicPolygon.png", width: 80%),
      [Comparaison de l'algorithme SNIC et SNIC Polygons]
    )
 ]
], fill: silver, stroke: black)
)

== Génération d'une grille de superpixel

Pour générer la grille de superpixel, nous utilisons l'algorithme SNIC [1] plus précisement sa version polygone afin d'obtenir des superpixels ayant des bords plus lisse pour éviter l'effet dentelé pouvant être observer sur le résultat ci dessous. Cet effet rendrait plus difficile la compréhension des imagettes placées car elles seraient trop déformées.

#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto, auto),
      rows: (auto, 1em),
      row-gutter: (1em),
      image("wpuech.jpg", width: 75%),
      image("superPixelFondImage.png", width: 75%),
      [Image originale],
      [Grille de superpixels obtenue],
    )
 ]
], fill: silver, stroke: black)
)

== Remplissage de la mosaïque

Afin de remplir la grille de superpixels obtenu, nous devons, une fois l'imagette idéal trouvé, lui appliqué un masque afin qu'elle remplisse uniquement la zone du superpixel. On applique cette opération sur tout les superpixel pour obtenir le résultat final.

#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto, auto, auto),
      rows: (auto, 1em),
      row-gutter: (1em),
      image("wpuech.jpg", width: 75%),
      image("superPixelFondImage.png", width: 75%),
      image("mosaiqueSuperPixel.png", width: 75%),
      [Image originale],
      [Grille de superpixels obtenue],
      [Mosaïque générée],
    )
 ]
], fill: silver, stroke: black)
)
= Références

[1] - Superpixels and Polygons using Simple Non-Iterative Clustering, Radhakrishna Achanta, Süsstrunk, Sabine, 2017