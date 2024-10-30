<!-- LTeX: enabled=true, language=en -->

# OOPetris Releases


We support multiple platforms and different types of executables / packages.
So here is a list of supported "package managers" / installation platforms.

## Android / FDroid

> [!NOTE]
> This is still WIP

## Linux / Flatpak

We provide stable and beta releases via 2 remotes:

To use the stable version, use this repo:

```bash
flatpak remote-add --if-not-exists oopetris-stable https://oopetris.totto.lt/repo/assets/oopetris.stable.flatpakrepo

```

And to use beta version:

```bash
flatpak remote-add --if-not-exists oopetris-beta https://oopetris.totto.lt/repo/assets/oopetris.beta.flatpakrepo

```

Than install it by using this command:

```bash
flatpak install -v io.github.openbrickprotocolfoundation.oopetris
```

To update use the normal flatpak update command, keep in mind, that the bet may be unstable and break things, use with caution!

## Other

> [!NOTE]
> This is still WIP

All other platforms have more difficult to support auto update capabilities, so you can find more on our [website](https://oopetris.totto.lt/releases).
