
# Welcome to the Netero's documentation

You will find all needed informations here. If you notice that something is missing,
please open an issue on github.

## Introduction

Netero help you create application with real time problematics,
events managment and deal with shared ressources.
Netero is a set of libraries, each library is considered as a module.


The project use cmake to generate and build the solution, it could be integrated
using FetchContentAdd macro or threw a git submodule in your own solution. If you
are not using cmake as build system, you can build and install netero seperatly.

# Features & Components 

Netero aims to expose monolithic containers and patterns working beside the STL
in the most generic way. We try to make as much as possible Netero features supported
under Windows, linux based OS and macOs.

## Core

The core containe basics standalone containers. It also provide patterns that might
be reuse in another submodules.

Containers:
 * avl: balanced binary search tree
 * shared buffer: thread safe circular buffer
 * set: std::set with isSubsetOf and interWith feature
 * type id: static type typeId container (needed for the ecs)

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
 * component filter: filter container base on entities's componet for systems

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

## Graphics

This submodul is work in progress.

# Motivation

Netero was orignaly developed for learning purpose, even thought today it is developed by
hobyist with an open-source mindset.

