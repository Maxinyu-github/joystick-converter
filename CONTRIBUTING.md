# Contributing to Joystick Converter

Thank you for your interest in contributing to the Joystick Converter project! This document provides guidelines and instructions for contributing.

## Code of Conduct

- Be respectful and inclusive
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Respect differing viewpoints and experiences

## Ways to Contribute

### 1. Report Bugs

If you find a bug, please create an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Firmware version and hardware details
- Serial output or error messages
- Screenshots if applicable

### 2. Suggest Features

Feature requests are welcome! Please include:
- Clear description of the feature
- Use cases and benefits
- Possible implementation approach
- Any relevant examples from other projects

### 3. Improve Documentation

Documentation improvements are always appreciated:
- Fix typos or unclear explanations
- Add examples and tutorials
- Translate documentation
- Improve API documentation
- Add troubleshooting tips

### 4. Submit Code

#### Before You Start

1. Check existing issues and PRs to avoid duplication
2. For major changes, open an issue first to discuss
3. Fork the repository
4. Create a feature branch

#### Development Setup

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/joystick-converter.git
cd joystick-converter

# Set up Pico SDK
export PICO_SDK_PATH=/path/to/pico-sdk

# Build
mkdir build && cd build
cmake ..
make
```

#### Coding Standards

**C/C++ Code (Firmware):**
- Follow existing code style
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small
- Use `static` for internal functions
- Initialize all variables
- Check return values

**Python Code (Configuration Software):**
- Follow PEP 8 style guide
- Use type hints where appropriate
- Write docstrings for functions
- Keep functions focused
- Handle exceptions properly

**Example C Style:**
```c
/**
 * Brief function description
 * 
 * @param input Description of input parameter
 * @return Description of return value
 */
static bool process_input(const gamepad_state_t *input) {
    if (!input) {
        return false;
    }
    
    // Implementation
    return true;
}
```

**Example Python Style:**
```python
def configure_device(port: str, config: dict) -> bool:
    """
    Configure device with provided settings.
    
    Args:
        port: Serial port name
        config: Configuration dictionary
        
    Returns:
        True on success, False on failure
    """
    if not port or not config:
        return False
    
    # Implementation
    return True
```

#### Commit Messages

Write clear, descriptive commit messages:

```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain what and why, not how.

- Bullet points are okay
- Use present tense ("Add feature" not "Added feature")
- Reference issues: "Fixes #123" or "Related to #456"
```

Examples:
- ✅ "Add support for Xbox Elite controller"
- ✅ "Fix button mapping not persisting after reboot"
- ✅ "Improve mouse sensitivity configuration"
- ❌ "Fixed stuff"
- ❌ "Update code"

#### Testing

Before submitting:

1. **Build Test**: Ensure firmware compiles without errors
   ```bash
   cd build
   make clean
   make
   ```

2. **Functionality Test**: Test on actual hardware
   - Flash firmware to device
   - Test affected features
   - Test edge cases
   - Verify no regressions

3. **Code Review**: Review your own changes
   - Remove debug code
   - Check for memory leaks
   - Verify error handling
   - Update documentation

#### Pull Request Process

1. **Update Documentation**: 
   - Update README if needed
   - Add/update API documentation
   - Update CHANGELOG

2. **Create Pull Request**:
   - Use a clear title
   - Describe changes in detail
   - Reference related issues
   - Include testing done
   - Add screenshots for UI changes

3. **Pull Request Template**:
   ```markdown
   ## Description
   Brief description of changes
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation update
   
   ## Testing
   - Describe testing performed
   - Hardware tested on
   - Edge cases considered
   
   ## Checklist
   - [ ] Code compiles without errors
   - [ ] Tested on hardware
   - [ ] Documentation updated
   - [ ] No breaking changes (or documented)
   
   ## Related Issues
   Fixes #123
   ```

4. **Review Process**:
   - Maintainers will review your PR
   - Address feedback and suggestions
   - Make requested changes
   - Be patient and respectful

## Project Structure

```
joystick-converter/
├── firmware/           # RP2350 firmware
│   ├── main.c         # Main entry point
│   ├── usb_host.*     # USB host functionality
│   ├── usb_device.*   # USB device functionality
│   ├── config.*       # Configuration management
│   ├── remapping.*    # Button remapping engine
│   └── macro.*        # Macro system
├── config_software/   # PC configuration tool
├── docs/              # Documentation
├── .github/           # GitHub workflows
└── CMakeLists.txt    # Build configuration
```

## Feature Development Guidelines

### Adding New Gamepad Support

1. Identify HID descriptor format
2. Add parsing code in `usb_host.c`
3. Update button mapping definitions
4. Test with actual hardware
5. Document in compatibility list

### Adding New Output Type

1. Define output type in `usb_device.h`
2. Implement HID descriptor
3. Add sending function in `usb_device.c`
4. Update configuration software
5. Add examples and documentation

### Adding New Macro Actions

1. Define action type in `macro.h`
2. Implement in `macro_task()` in `macro.c`
3. Update configuration software UI
4. Add to examples documentation
5. Test thoroughly

## Documentation Guidelines

- Use clear, simple language
- Include examples
- Add diagrams where helpful
- Keep formatting consistent
- Test all code examples
- Update table of contents if needed

## Release Process

Maintainers follow this process for releases:

1. Update version numbers
2. Update CHANGELOG
3. Create release branch
4. Final testing
5. Create GitHub release
6. Build and upload firmware artifacts
7. Announce release

## Questions?

- Open an issue for general questions
- Join discussions in existing issues
- Check documentation first
- Be patient and respectful

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS file
- Credited in release notes
- Mentioned in relevant documentation

Thank you for contributing to Joystick Converter! 🎮
