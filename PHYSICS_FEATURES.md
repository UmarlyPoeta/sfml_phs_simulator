# Advanced Physics Features

This physics simulator now includes multiple material types with realistic interactions:

## Materials Available

### Sand (S Key)
- **Color**: Yellow/brown
- **Physics**: Falls due to gravity with velocity accumulation
- **Density**: High (displaces water and gas)
- **Special**: Can catch fire from adjacent fire particles

### Wall (W Key) 
- **Color**: Gray
- **Physics**: Static, immovable barriers
- **Density**: Infinite (cannot be displaced)
- **Special**: Blocks all other materials

### Water (L Key)
- **Color**: Blue
- **Physics**: Falls due to gravity, flows horizontally when blocked
- **Density**: Medium (displaces gas, displaced by sand)
- **Special**: Realistic liquid behavior with pooling

### Gas (G Key)
- **Color**: Light gray (semi-transparent)
- **Physics**: Rises due to buoyancy, has horizontal drift
- **Density**: Low (displaced by all other materials)
- **Special**: Chaotic movement with random drift

### Fire (F Key)
- **Color**: Orange-red with flickering effect
- **Physics**: Rises quickly like gas, spreads to nearby flammable materials
- **Density**: Very low
- **Special**: 
  - Has limited lifetime before burning out
  - Spreads to adjacent sand particles
  - More chaotic movement than gas

## Physics Systems

### Velocity-Based Movement
- All particles accumulate velocity over time
- More realistic falling and movement patterns
- Maximum velocity limits prevent unrealistic speeds

### Density-Based Interactions
- Heavier materials sink through lighter ones
- Sand > Water > Fire > Gas in terms of density
- Creates realistic layering effects

### Material-Specific Behaviors
- **Liquids** flow horizontally when blocked vertically
- **Gases** rise and drift with buoyancy
- **Fire** spreads to flammable materials and has lifetime
- **Solids** fall and settle realistically

### Advanced Features
- Fire propagation system
- Material displacement based on density
- Realistic fluid dynamics for water
- Gas dispersion and drift patterns

## Controls
- **S** - Draw Sand
- **W** - Draw Wall  
- **L** - Draw Water (Liquid)
- **G** - Draw Gas
- **F** - Draw Fire
- **E** - Erase
- **Left Mouse** - Draw selected material

## Building and Running
```bash
mkdir build
cd build
cmake ..
make
./sfml_phs_simulator
```

Requires SFML 2.5+ development libraries.