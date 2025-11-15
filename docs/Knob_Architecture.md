# Knob Component Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Widget (Base Class)                      │
│  - virtual draw()                                                │
│  - virtual handleEvent()                                         │
│  - virtual setPosition()                                         │
│  - virtual setScale()                                            │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             │ inherits
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                        Knob Component                            │
├─────────────────────────────────────────────────────────────────┤
│ Public Interface:                                                │
│  • Knob(cx, cy, radius)                     [Constructor]        │
│  • setRange(min, max)                       [Core Config]        │
│  • setStep(step)                            [Core Config]        │
│  • setValue(val) / getValue()               [Value Control]      │
│  • setColor(fg, bg)                         [Appearance]         │
│  • setOffsetAngle(angle)                    [Advanced]           │
│  • setInnerRange(innerMin, innerMax)        [Advanced]           │
│  • setShowValue(show)                       [Display]            │
│  • setFontSize(size)                        [Display]            │
│  • setDisabled(disabled)                    [State]              │
│  • setReadonly(readonly)                    [State]              │
│  • setOnChange(callback)                    [Events]             │
│  • setScale(scale)                          [Transform]          │
│  • setPosition(x, y)                        [Transform]          │
│  • draw(dst, x, y)                          [Rendering]          │
│  • handleEvent(msg)                         [Interaction]        │
├─────────────────────────────────────────────────────────────────┤
│ Private Implementation:                                          │
│  • clamp(v) const                           [Utility]            │
│  • valueToAngle(v) const                    [Conversion]         │
│  • angleToValue(angle) const                [Conversion]         │
│  • applyStep(v) const                       [Utility]            │
│  • isInside(x, y) const                     [Hit Test]           │
│  • calculateAngle(x, y) const               [Geometry]           │
├─────────────────────────────────────────────────────────────────┤
│ State Variables:                                                 │
│  • cx, cy                    - Center position                   │
│  • radius, origin_radius     - Size (current & original)         │
│  • scale                     - Scale factor                      │
│  • minValue, maxValue        - Value range                       │
│  • step, value               - Current value & step              │
│  • offsetAngle               - Arc rotation offset               │
│  • innerMin, innerMax        - Inner range limits                │
│  • showValue, fontSize       - Display config                    │
│  • disabled, readonly        - State flags                       │
│  • fgColor, bgColor          - Colors                            │
│  • dragging, lastMouseX/Y    - Interaction state                 │
│  • hovered                   - Visual state                      │
│  • onChange                  - Callback function                 │
└─────────────────────────────────────────────────────────────────┘
                             ▲
                             │ builds
                             │
┌─────────────────────────────────────────────────────────────────┐
│                      KnobBuilder (Builder Pattern)               │
├─────────────────────────────────────────────────────────────────┤
│ Fluent API Methods:                                              │
│  • setIdentifier(id)                        [Registration]       │
│  • setCenter(x, y)                          [Position]           │
│  • setRadius(r)                             [Size]               │
│  • setRange(min, max)                       [Value]              │
│  • setStep(step)                            [Value]              │
│  • setValue(val)                            [Value]              │
│  • setColor(fg, bg)                         [Appearance]         │
│  • setOffsetAngle(angle)                    [Advanced]           │
│  • setInnerRange(innerMin, innerMax)        [Advanced]           │
│  • setShowValue(show)                       [Display]            │
│  • setFontSize(size)                        [Display]            │
│  • setDisabled(disabled)                    [State]              │
│  • setReadonly(readonly)                    [State]              │
│  • setScale(scale)                          [Transform]          │
│  • setOnChange(callback)                    [Events]             │
│  • build()                                  [Factory]            │
├─────────────────────────────────────────────────────────────────┤
│ Builder State:                                                   │
│  - Stores all configuration parameters                           │
│  - Applies them during build()                                   │
│  - Registers widget globally                                     │
│  - Maps identifier to widget                                     │
└─────────────────────────────────────────────────────────────────┘
```

## Component Interaction Flow

```
┌──────────┐
│   User   │
└────┬─────┘
     │
     │ (1) Create via Builder
     ▼
┌─────────────────────┐
│  KnobBuilder()      │
│  .setCenter(x,y)    │
│  .setRadius(r)      │
│  .setRange(min,max) │
│  .setValue(val)     │
│  .build()           │
└─────────┬───────────┘
          │
          │ (2) Constructs & Configures
          ▼
     ┌────────┐
     │  Knob  │
     └────┬───┘
          │
          │ (3) Auto-registers
          ▼
┌──────────────────────┐
│ Global Widget System │
│  - widgets set       │
│  - IdToWidget map    │
└──────────────────────┘
```

## Event Flow (Mouse Interaction)

```
     Mouse Event
         │
         ▼
   ┌─────────────┐
   │ handleEvent │
   └──────┬──────┘
          │
          ├─→ (Check) isInside(x, y) ?
          │      │
          │      ▼
          │   ┌──────────┐
          │   │  Hovered │
          │   └──────────┘
          │
          ├─→ (Left Down & Inside) ?
          │      │
          │      ▼
          │   ┌──────────────┐
          │   │ Start Drag   │
          │   │ dragging=true│
          │   └──────────────┘
          │
          ├─→ (Move & Dragging) ?
          │      │
          │      ▼
          │   ┌────────────────────┐
          │   │ Calculate Delta Y  │
          │   │ Update Value       │
          │   │ Apply Step         │
          │   │ Trigger onChange   │
          │   └────────────────────┘
          │
          └─→ (Left Up) ?
                 │
                 ▼
              ┌────────────────┐
              │  Stop Drag     │
              │ dragging=false │
              └────────────────┘
```

## Rendering Pipeline

```
     draw(dst, x, y)
         │
         ├─→ Calculate scaled radius
         │
         ├─→ Draw Background Arc (280°)
         │      │
         │      └─→ ege_arc(track color)
         │
         ├─→ Draw Progress Arc
         │      │
         │      ├─→ valueToAngle(value)
         │      └─→ ege_arc(foreground color)
         │
         ├─→ Draw Center Circle
         │      │
         │      └─→ fillellipse(white/hover)
         │
         └─→ Draw Value Text (if showValue)
                │
                ├─→ Format value based on step
                ├─→ Calculate font size
                ├─→ Center text
                └─→ outtextxy(text)
```

## Value Conversion

```
User Input (drag)
     │
     ▼
┌─────────────┐
│  deltaY     │ (pixels moved)
└──────┬──────┘
       │
       │ (sensitivity: range/100)
       ▼
┌─────────────┐
│ deltaValue  │
└──────┬──────┘
       │
       │ + current value
       ▼
┌─────────────┐
│  newValue   │
└──────┬──────┘
       │
       │ clamp(min, max)
       ▼
┌─────────────┐
│  clamped    │
└──────┬──────┘
       │
       │ applyStep() if step > 0
       ▼
┌─────────────┐
│ Final Value │
└──────┬──────┘
       │
       │ if changed
       ▼
┌─────────────┐
│  onChange() │ (callback)
└─────────────┘
```

## Angle Mapping

```
Value Range: [min, max]
     │
     │ normalize: (value - min) / (max - min)
     ▼
Ratio: [0.0, 1.0]
     │
     │ map to angle range
     ▼
Angle: [-140°, +140°] + offsetAngle
     │
     │ Total sweep: 280°
     ▼
Arc Position on Circle

Example:
  value = min     → angle = -140° + offset
  value = mid     → angle =    0° + offset  
  value = max     → angle = +140° + offset
```

## Memory Management

```
┌─────────────┐
│ new Knob()  │
└──────┬──────┘
       │
       │ (auto)
       ▼
┌──────────────────┐
│ widgets.insert() │ (global set)
└──────┬───────────┘
       │
       │ (if has ID)
       ▼
┌──────────────────────────┐
│ IdToWidget[id] = widget  │
└──────┬───────────────────┘
       │
       │ ... widget lifetime ...
       │
       ▼
┌─────────────┐
│ ~Knob()     │ (destructor)
└──────┬──────┘
       │
       ├─→ widgets.erase(this)
       │
       └─→ IdToWidget cleanup
```

## Dependencies

```
Knob Component
    │
    ├─→ Widget (base class)
    │     │
    │     └─→ Element?
    │
    ├─→ Base.h
    │     │
    │     ├─→ clamp<T>() template
    │     ├─→ Position class
    │     └─→ FeEGE namespace
    │
    └─→ EGE Graphics Library
          │
          ├─→ ege_arc()
          ├─→ fillellipse()
          ├─→ outtextxy()
          ├─→ setlinecolor()
          ├─→ setfillcolor()
          └─→ color_t type
```

## Design Patterns Used

1. **Widget Pattern** - Base class for all UI components
2. **Builder Pattern** - Fluent API for construction
3. **Observer Pattern** - onChange callback system
4. **State Pattern** - disabled/readonly/normal states
5. **Template Method** - Virtual methods (draw, handleEvent)
6. **Singleton-like** - Global widgets registry

## Key Design Decisions

1. **Vertical Drag** - Chosen over radial drag for easier control
2. **280° Arc** - Matches Quasar QKnob default range
3. **Center Position** - Consistent with other widgets
4. **Auto Registration** - Simplifies memory management
5. **Step Snapping** - Applied during drag for smooth interaction
6. **Smart Formatting** - Based on step size for clarity
7. **Origin Preservation** - Enables proper scaling
