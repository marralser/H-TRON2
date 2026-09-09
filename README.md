H-Tron2

A classic DOS light-cycle game, extended for late-1980s PC hardware.

H-Tron is a small DOS game inspired by the light-cycle battles of *Tron*. This edition is based on the original **H-TRON by Robert K in 2016**, whose compact Turbo C game provided the foundation for the project.

In 2026, the game was extended with a simple goal: rather than modernizing H-Tron into a modern PC game, develop it further as if it had evolved into a more complete DOS game in the late 1980s.

The result stays close to its Turbo C and XT-class roots while adding color graphics, sound, a rare Amstrad PC1512 / PC1640 joystick support, multiple levels, scoring and some increasingly chaotic opponents.


The 2026 edition was developed and compiled using **Borland Turbo C under DOS** on a real Amstrad PC1640, but ChatCPT supported the coding.

* NEC V30 CPU at 8 MHz
* 640 KB RAM
* VGA graphics
* AdLib-compatible FM sound
* Amstrad digital PC joystick

The game has also been tested with slower XT-class hardware, including a 4.77 MHz 8088 IBM PC 5150, which influenced several optimizations in the graphics and game logic.

There are no modern frameworks hiding underneath: H-Tron remains a DOS/Turbo C program designed around the limitations and character of period PC hardware.

## Gameplay

You control a Tron that continuously moves across the arena, leaving an impassable trail behind it.

Steer **left or right** and try to survive longer than your opponent. Hitting a wall, a trail, an obstacle or one of the later hazards costs the round.

Win a **best-of-three** match to advance to the next level.

The game supports:

* **Demo Mode** — watch the computer play itself
* **One Player** — compete through the multi-level campaign
* **Two Players** — battle another human player
* **Keyboard control**
* **Amstrad PC1512/PC1640 digital joystick control**

## Five levels

**Level 1 — The original battle**

A straightforward light-cycle duel against the computer. Survive your opponent to win the round.

**Level 2 — Energy Ball**

A fast bouncing energy ball enters the arena. It bounces from the walls and Tron trails without leaving a trail of its own. Touch it and you lose.

**Level 3 — Obstacles**

Green obstacles periodically appear at random free positions in the arena. As the round continues, the available space becomes increasingly restricted.

**Level 4 — Three Computers**

One of the more frantic challenges: the human player faces **three computer-controlled Trons simultaneously**. Outlive all three to win the round.

**Level 5 — Everything**

The final level combines the previous hazards: three computer Trons, the bouncing energy ball and randomly appearing obstacles all share the arena.

Good luck.

## Graphics

The game runs in **640×480 VGA mode** with a centered, EGA-sized playing arena.

Color is deliberately used in the style of a late-1980s DOS game rather than attempting a modern graphical makeover. The Trons, trails, obstacles and hazards use the familiar bright palette of period PC graphics.

The opening screen uses CP437/ANSI artwork, continuing the DOS aesthetic before the game switches into graphics mode.

## AdLib sound

The extended edition supports **AdLib / Yamaha OPL2 FM sound**.

Rather than sampled audio, H-Tron writes directly to the classic AdLib registers at ports `388h/389h`. Short synthesized effects accompany turns, collisions, round starts, hazards and level changes.

The victory sound is a simple C-major **C–E–G–C** FM fanfare.

There is deliberately no background music. The sparse electronic effects fit both the game and the limited hardware it was designed for.

## Scoring and Hall of Fame

The one-player game includes a scoring system and persistent **Top 10 Hall of Fame**.

Points are awarded for surviving, making turns and defeating opponents. High scores and player names are stored in a simple DOS text file.

## Why this project?

H-Tron is partly a game and partly an experiment in developing software within the constraints of an earlier PC era.

Instead of asking:

> “How would we remake this game today?”

the project asks:

> **“What might H-Tron have become if it had been expanded into a fuller DOS game on late-1980s hardware?”**

That means keeping Borland Turbo C, direct hardware access, BGI graphics, OPL2 sound, 640 KB memory constraints, an 8 MHz processor and a real digital joystick—not replacing them.

A surprising amount of gameplay fits inside those limitations.

## Origins and credits

**H-TRON — Original game and source:** Robert K
**Extended edition (2026):** Markus R

The 2026 edition retains H-Tron's original concept and Turbo C heritage while extending it with VGA color graphics, Amstrad joystick support, AdLib sound effects, scoring and Hall of Fame, a multi-level campaign, additional computer opponents, moving hazards, obstacles, ANSI artwork and numerous compatibility and performance improvements for XT-class systems.

Most importantly, the project remains what H-Tron started as:

**a small, fast DOS game made for old PCs.**
