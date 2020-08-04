
# Welcome to the Netero's documentation

Many things are still work in progress this documentation might be very light and confusing as things move on frequently.
A great place to start is the samples folder.

## Introduction

Netero help you create application with real time problematics,
events management and deal with shared resources.
Netero is a set of libraries, each library is considered as a module.

The project use cmake to generate and build the solution, it could be integrated
using FetchContentAdd macro or threw a git submodule in your own solution. If you
are not using cmake as build system, you can build and install netero separately.

# Features & Components 

Netero aims to expose monolithic containers and patterns working beside the STL
in the most generic way. We try to make as much as possible Netero features supported
under Windows, linux based OS and macOs.

## Core

The core contain basics standalone containers. It also provide patterns that might be reuse in another submodules.

Algorithm container:
 * avl: balanced binary search tree
 * shared buffer: thread safe circular buffer
 * set: std::set with isSubsetOf and interWith feature
 
Runtime typeid independent from RTTI (needed by the ECS):
 * type id: static type typeId container

## Patterns

This submodule contain patterns to help you fast prototype with reasonable  

ECS pattern containers:
 * world: ecs context container holder
 * entity: entity container managed by a word container
 * system: logic holder container fot entities
 * component: attributes holder container for entities
 * component filter: filter container base on entities component for systems
 
Signal/Slot containers based on IObserver:
 * slot: a callback holder container
 * signal: an event emitter container to connect with slots 
 
Interfaces:
 * IFactory: Create factories to spawn objects without specifying the base class
 * IObserver/ISubject: Observer pattern to let objects notify each others of events
 * IConnectibleDelegate: Interface to manage object dependency at runtime between each others

## Audio

The audio submodule has got containers to provide fast and raw access to audio buffers.
Loopback recording devices is supported on Windows using the WASAPI.

Container:
 * Device: An audio device
 * DeviceManager: Scan and manage audio devices

## fast

Make things fast obviously^^
Big WIP

## Graphics

WIP

The graphics submodule provide container to perform procing on GPU usgin graphics pipelines or computes pipilines.
It rely on the Vulkan SDK but do not redisribute it.
In order to use this module you just need to install it, Netero's cmake macros takes care of the rest.

## memcheck

This module is essentially for diagnostics and debug purpose.

Global Allocator:
 * new/delete: Override default allocator to keep track of leaks and potential buffer overflows
 
 Allocator:
  * Allocator: Default diagnostic allocator for stl and netero container that support templated allocators
  * LoggerAllocator: Like Allocator but with tones of logs, may considerably reduce performance

## Extra

The extra module contain self hosted code that does not belong to Netero, which means that it is not
covered by the Netero's license and is subject to terms and conditions fixed by theirs respective authors.
Extra is a way to redistributes this sources to other Netero's modules, while theirs license permit it.

stb:
 * stb_image
 * stb_image_resize
 * stb_image_write

# Motivation

Netero was originally developed for learning purpose, even thought today it is developed by hobbyist with an open-source mindset.
