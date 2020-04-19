
![Logo](docs/logo/logo+text.png)

[![CircleCI](https://circleci.com/gh/domage-j/netero.svg?style=svg)](https://circleci.com/gh/domage-j/netero)
[![Documentation](https://codedocs.xyz/domage-j/netero.svg)](https://codedocs.xyz/domage-j/netero/)
[![codecov](https://codecov.io/gh/domage-j/netero/branch/develop/graph/badge.svg)](https://codecov.io/gh/domage-j/netero)
![Discord](https://img.shields.io/discord/621011079417298944?color=blue&label=Netero&logo=discord)

Netero's libs help you create application with real-time problematics, events managements and deal with shared ressources.


Netero aims to expose monolithic containers and patterns working beside the STL
in the most generic way. I tried to make as much as possible Netero features supported
under Windows, linux based OS and macOs.

You can access to the documentation by clicking on the "documented" badge.
Feel free to join our discord channel to chat with us.

## Core

The core containe basics standalone containers. It also provide patterns that might
be reuse in another submodules.

Containers:
 * avl: balanced binary search tree
 * shared_buffer: thread safe circular buffer
 * set: std::set with isSubsetOf and interWith feature
 * type_id: static type typeId container (needed for the ecs)

The core contain an Observer pattern, with the following container:
 * slot: a callback holder container
 * signal: an event emitter container to connect with slots

## ECS

The Ecs submodule contain an Entity Component System pattern ready to use.

Containers:
 * world: ecs context container holder
 * entity: entity container managed by a wolrd container
 * system: logic holder container fot entities
 * component: attributs holder container for entities
 * component_filter: filters container base on entities's components for systems

## Audio

The audio submodule contain a very light audio engine.

Container:
 * engine: container exposing the specific OS implementation
 * device: container representing an audio device
 * mixer: container mixing multiple signal into one
 * renderStream: interface to be implemented by the client to render audio data
 * captureStream: interface to be implemented by the client to capture audio data
 * signals: containers ready to render signals
 * waveRecorder: container to record audio into a wave file


