# Reinforcement Learning in Helicopter Game
This is a proof of concept RL implementation for a helicopter game. The helicopter learns to avoid obstacles by learning a reward function based on Radial Basis Clustering.

<p align="center">
  <img src="https://raw.githubusercontent.com/AmirooR/RL_helicoopter/master/COPS.gif" alt="Demo"/>
</p>

## Compile
You should have qt4 or qt5 and a c++ compiler. Type the following commands in shell:

    qmake
    make

## Usage
Run the runnable output of make. Then,
```html
    Press
        <space> to pause/resume
        <S>     to save clusters
        <P>     to switch to autpilot mode
        <I>     to save episode
        <L>     to load clusters
        <G>     to start learning in background mode
        <R>     to switch to replay mode (then use forward and backward to jump 1000 episodes forward or backward
```

### Train on background and replay
1. Press 'I' to enable saving episodes. They will be saved in './episodes/' folder.
2. Press 'G' to run background training and wait until finishing...
3. When it is done, press 'R' to replay. press left/right arrow keys to move 1000 episodes back/forward in the player.
