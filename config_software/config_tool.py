#!/usr/bin/env python3
"""
Joystick Converter Configuration Tool

GUI application for configuring the joystick converter device.
"""

import sys
import time
import serial
import serial.tools.list_ports
from datetime import datetime
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QComboBox, QLabel, QTableWidget, QTableWidgetItem,
    QMessageBox, QGroupBox, QDialog, QDialogButtonBox, QFormLayout,
    QSpinBox, QTextEdit, QTabWidget, QProgressBar, QCheckBox, QGridLayout,
    QFileDialog, QPlainTextEdit
)
from PyQt6.QtCore import Qt, QTimer
from PyQt6.QtGui import QFont, QTextCursor


class MacroEditorDialog(QDialog):
    """Dialog for editing macros"""
    
    def __init__(self, parent=None, macro_data=None):
        super().__init__(parent)
        self.setWindowTitle("Macro Editor")
        self.setMinimumWidth(600)
        
        layout = QVBoxLayout()
        
        # Macro ID
        form_layout = QFormLayout()
        self.macro_id_spin = QSpinBox()
        self.macro_id_spin.setRange(0, 15)
        if macro_data:
            self.macro_id_spin.setValue(macro_data.get('id', 0))
        form_layout.addRow("Macro ID:", self.macro_id_spin)
        layout.addLayout(form_layout)
        
        # Macro steps
        layout.addWidget(QLabel("Macro Steps (one per line):"))
        self.steps_text = QTextEdit()
        self.steps_text.setPlaceholderText(
            "Example format:\n"
            "key_press 0x04 # A key\n"
            "delay 100\n"
            "key_release\n"
            "mouse_move 10 10\n"
            "mouse_button_press 1\n"
            "delay 50\n"
            "mouse_button_release"
        )
        if macro_data and 'steps' in macro_data:
            self.steps_text.setPlainText(macro_data['steps'])
        layout.addWidget(self.steps_text)
        
        # Buttons
        button_box = QDialogButtonBox(
            QDialogButtonBox.StandardButton.Ok | 
            QDialogButtonBox.StandardButton.Cancel
        )
        button_box.accepted.connect(self.accept)
        button_box.rejected.connect(self.reject)
        layout.addWidget(button_box)
        
        self.setLayout(layout)
    
    def get_macro_data(self):
        """Get macro data from dialog"""
        return {
            'id': self.macro_id_spin.value(),
            'steps': self.steps_text.toPlainText()
        }


class ConfigTool(QMainWindow):
    """Main configuration window"""
    
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.button_mappings = []
        self.macros = []
        self.debug_mode_active = False
        self.last_gamepad_state = None
        
        self.init_ui()
        self.refresh_ports()
        
        # Auto-refresh port list
        self.port_refresh_timer = QTimer()
        self.port_refresh_timer.timeout.connect(self.refresh_ports)
        self.port_refresh_timer.start(2000)  # Refresh every 2 seconds
        
        # Debug mode polling timer
        self.debug_timer = QTimer()
        self.debug_timer.timeout.connect(self.poll_debug_data)
        self.debug_timer.setInterval(50)  # Poll every 50ms for responsive input
    
    def init_ui(self):
        """Initialize user interface"""
        self.setWindowTitle("Joystick Converter Configuration Tool")
        self.setMinimumSize(900, 700)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Connection section (always visible at top)
        connection_group = QGroupBox("Connection")
        connection_layout = QHBoxLayout()
        
        connection_layout.addWidget(QLabel("Port:"))
        self.port_combo = QComboBox()
        connection_layout.addWidget(self.port_combo)
        
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_button)
        
        self.refresh_button = QPushButton("Refresh")
        self.refresh_button.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(self.refresh_button)
        
        connection_layout.addStretch()
        connection_group.setLayout(connection_layout)
        main_layout.addWidget(connection_group)
        
        # Create tab widget
        self.tab_widget = QTabWidget()
        main_layout.addWidget(self.tab_widget)
        
        # Configuration Tab
        config_tab = QWidget()
        config_layout = QVBoxLayout(config_tab)
        
        # Output type section
        output_group = QGroupBox("Output Type")
        output_layout = QHBoxLayout()
        
        output_layout.addWidget(QLabel("Output Device:"))
        self.output_combo = QComboBox()
        self.output_combo.addItems(["Gamepad", "Keyboard", "Mouse", "Keyboard+Mouse"])
        self.output_combo.currentIndexChanged.connect(self.on_output_changed)
        output_layout.addWidget(self.output_combo)
        
        output_layout.addStretch()
        output_group.setLayout(output_layout)
        config_layout.addWidget(output_group)
        
        # Button mapping section
        mapping_group = QGroupBox("Button Mappings")
        mapping_layout = QVBoxLayout()
        
        self.mapping_table = QTableWidget()
        self.mapping_table.setColumnCount(4)
        self.mapping_table.setHorizontalHeaderLabels([
            "Source Button", "Mapping Type", "Target", "Action"
        ])
        mapping_layout.addWidget(self.mapping_table)
        
        mapping_buttons = QHBoxLayout()
        add_mapping_btn = QPushButton("Add Mapping")
        add_mapping_btn.clicked.connect(self.add_mapping)
        mapping_buttons.addWidget(add_mapping_btn)
        
        remove_mapping_btn = QPushButton("Remove Mapping")
        remove_mapping_btn.clicked.connect(self.remove_mapping)
        mapping_buttons.addWidget(remove_mapping_btn)
        
        mapping_buttons.addStretch()
        mapping_layout.addLayout(mapping_buttons)
        
        mapping_group.setLayout(mapping_layout)
        config_layout.addWidget(mapping_group)
        
        # Macro section
        macro_group = QGroupBox("Macros")
        macro_layout = QVBoxLayout()
        
        self.macro_table = QTableWidget()
        self.macro_table.setColumnCount(3)
        self.macro_table.setHorizontalHeaderLabels([
            "Macro ID", "Steps", "Action"
        ])
        macro_layout.addWidget(self.macro_table)
        
        macro_buttons = QHBoxLayout()
        add_macro_btn = QPushButton("Add Macro")
        add_macro_btn.clicked.connect(self.add_macro)
        macro_buttons.addWidget(add_macro_btn)
        
        edit_macro_btn = QPushButton("Edit Macro")
        edit_macro_btn.clicked.connect(self.edit_macro)
        macro_buttons.addWidget(edit_macro_btn)
        
        remove_macro_btn = QPushButton("Remove Macro")
        remove_macro_btn.clicked.connect(self.remove_macro)
        macro_buttons.addWidget(remove_macro_btn)
        
        macro_buttons.addStretch()
        macro_layout.addLayout(macro_buttons)
        
        macro_group.setLayout(macro_layout)
        config_layout.addWidget(macro_group)
        
        # Control buttons for config tab
        control_layout = QHBoxLayout()
        
        load_btn = QPushButton("Load from Device")
        load_btn.clicked.connect(self.load_config)
        control_layout.addWidget(load_btn)
        
        save_btn = QPushButton("Save to Device")
        save_btn.clicked.connect(self.save_config)
        control_layout.addWidget(save_btn)
        
        control_layout.addStretch()
        config_layout.addLayout(control_layout)
        
        self.tab_widget.addTab(config_tab, "Configuration")
        
        # Debug Mode Tab
        debug_tab = QWidget()
        debug_layout = QVBoxLayout(debug_tab)
        
        # Debug control section
        debug_control_group = QGroupBox("Debug Control")
        debug_control_layout = QHBoxLayout()
        
        self.debug_start_btn = QPushButton("Start Debug Mode")
        self.debug_start_btn.clicked.connect(self.toggle_debug_mode)
        debug_control_layout.addWidget(self.debug_start_btn)
        
        self.debug_status_label = QLabel("Status: Not Active")
        debug_control_layout.addWidget(self.debug_status_label)
        
        debug_control_layout.addStretch()
        debug_control_group.setLayout(debug_control_layout)
        debug_layout.addWidget(debug_control_group)
        
        # Info label
        info_label = QLabel(
            "Debug mode displays real-time joystick input. "
            "This helps identify which buttons your gamepad supports, "
            "including special buttons like joystick presses and back buttons."
        )
        info_label.setWordWrap(True)
        debug_layout.addWidget(info_label)
        
        # Create debug display
        self.init_debug_display(debug_layout)
        
        self.tab_widget.addTab(debug_tab, "Debug Mode")
        
        # Device Logs Tab
        logs_tab = QWidget()
        logs_layout = QVBoxLayout(logs_tab)
        
        # Log control section
        log_control_group = QGroupBox("Log Control")
        log_control_layout = QHBoxLayout()
        
        self.log_refresh_btn = QPushButton("Refresh Logs")
        self.log_refresh_btn.clicked.connect(self.refresh_logs)
        log_control_layout.addWidget(self.log_refresh_btn)
        
        self.log_clear_btn = QPushButton("Clear Logs")
        self.log_clear_btn.clicked.connect(self.clear_logs)
        log_control_layout.addWidget(self.log_clear_btn)
        
        self.log_export_btn = QPushButton("Export Logs")
        self.log_export_btn.clicked.connect(self.export_logs)
        log_control_layout.addWidget(self.log_export_btn)
        
        log_control_layout.addStretch()
        
        # Log level selector
        log_control_layout.addWidget(QLabel("Log Level:"))
        self.log_level_combo = QComboBox()
        self.log_level_combo.addItems(["DEBUG", "INFO", "WARN", "ERROR"])
        self.log_level_combo.setCurrentIndex(1)  # Default to INFO
        self.log_level_combo.currentIndexChanged.connect(self.set_log_level)
        log_control_layout.addWidget(self.log_level_combo)
        
        log_control_group.setLayout(log_control_layout)
        logs_layout.addWidget(log_control_group)
        
        # Log status section
        log_status_group = QGroupBox("Log Status")
        log_status_layout = QHBoxLayout()
        
        self.log_count_label = QLabel("Entries: 0")
        log_status_layout.addWidget(self.log_count_label)
        
        self.log_overflow_label = QLabel("Overflow: No")
        log_status_layout.addWidget(self.log_overflow_label)
        
        self.log_status_btn = QPushButton("Get Status")
        self.log_status_btn.clicked.connect(self.get_log_status)
        log_status_layout.addWidget(self.log_status_btn)
        
        log_status_layout.addStretch()
        log_status_group.setLayout(log_status_layout)
        logs_layout.addWidget(log_status_group)
        
        # Log display area
        log_display_group = QGroupBox("Device Logs")
        log_display_layout = QVBoxLayout()
        
        self.log_text = QPlainTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        font = QFont("Courier New", 9)
        self.log_text.setFont(font)
        self.log_text.setPlaceholderText(
            "Device logs will appear here.\n"
            "Click 'Refresh Logs' to fetch logs from the device."
        )
        log_display_layout.addWidget(self.log_text)
        
        log_display_group.setLayout(log_display_layout)
        logs_layout.addWidget(log_display_group)
        
        self.tab_widget.addTab(logs_tab, "Device Logs")
    
    def init_debug_display(self, parent_layout):
        """Initialize the debug display widgets"""
        # Button status group
        buttons_group = QGroupBox("Button States")
        buttons_layout = QGridLayout()
        
        # Create checkboxes for each button
        self.button_widgets = {}
        button_names = [
            ("A", 0, 0), ("B", 0, 1), ("X", 0, 2), ("Y", 0, 3),
            ("LB", 1, 0), ("RB", 1, 1), ("Back", 1, 2), ("Start", 1, 3),
            ("LS", 2, 0), ("RS", 2, 1)
        ]
        
        for name, row, col in button_names:
            checkbox = QCheckBox(name)
            checkbox.setEnabled(False)
            font = QFont()
            font.setBold(True)
            font.setPointSize(10)
            checkbox.setFont(font)
            buttons_layout.addWidget(checkbox, row, col)
            self.button_widgets[name] = checkbox
        
        buttons_group.setLayout(buttons_layout)
        parent_layout.addWidget(buttons_group)
        
        # Analog sticks group
        analog_group = QGroupBox("Analog Sticks")
        analog_layout = QGridLayout()
        
        # Left stick
        analog_layout.addWidget(QLabel("Left Stick:"), 0, 0)
        self.left_x_label = QLabel("X: 0")
        self.left_y_label = QLabel("Y: 0")
        analog_layout.addWidget(self.left_x_label, 0, 1)
        analog_layout.addWidget(self.left_y_label, 0, 2)
        
        self.left_x_bar = QProgressBar()
        self.left_x_bar.setRange(-32768, 32767)
        self.left_x_bar.setValue(0)
        analog_layout.addWidget(self.left_x_bar, 1, 1)
        
        self.left_y_bar = QProgressBar()
        self.left_y_bar.setRange(-32768, 32767)
        self.left_y_bar.setValue(0)
        analog_layout.addWidget(self.left_y_bar, 1, 2)
        
        # Right stick
        analog_layout.addWidget(QLabel("Right Stick:"), 2, 0)
        self.right_x_label = QLabel("X: 0")
        self.right_y_label = QLabel("Y: 0")
        analog_layout.addWidget(self.right_x_label, 2, 1)
        analog_layout.addWidget(self.right_y_label, 2, 2)
        
        self.right_x_bar = QProgressBar()
        self.right_x_bar.setRange(-32768, 32767)
        self.right_x_bar.setValue(0)
        analog_layout.addWidget(self.right_x_bar, 3, 1)
        
        self.right_y_bar = QProgressBar()
        self.right_y_bar.setRange(-32768, 32767)
        self.right_y_bar.setValue(0)
        analog_layout.addWidget(self.right_y_bar, 3, 2)
        
        analog_group.setLayout(analog_layout)
        parent_layout.addWidget(analog_group)
        
        # Triggers group
        triggers_group = QGroupBox("Triggers")
        triggers_layout = QGridLayout()
        
        triggers_layout.addWidget(QLabel("Left Trigger:"), 0, 0)
        self.left_trigger_label = QLabel("0")
        triggers_layout.addWidget(self.left_trigger_label, 0, 1)
        
        self.left_trigger_bar = QProgressBar()
        self.left_trigger_bar.setRange(0, 255)
        self.left_trigger_bar.setValue(0)
        triggers_layout.addWidget(self.left_trigger_bar, 1, 0, 1, 2)
        
        triggers_layout.addWidget(QLabel("Right Trigger:"), 2, 0)
        self.right_trigger_label = QLabel("0")
        triggers_layout.addWidget(self.right_trigger_label, 2, 1)
        
        self.right_trigger_bar = QProgressBar()
        self.right_trigger_bar.setRange(0, 255)
        self.right_trigger_bar.setValue(0)
        triggers_layout.addWidget(self.right_trigger_bar, 3, 0, 1, 2)
        
        triggers_group.setLayout(triggers_layout)
        parent_layout.addWidget(triggers_group)
        
        # D-pad group
        dpad_group = QGroupBox("D-Pad")
        dpad_layout = QHBoxLayout()
        
        self.dpad_label = QLabel("Center")
        font = QFont()
        font.setPointSize(12)
        font.setBold(True)
        self.dpad_label.setFont(font)
        self.dpad_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        dpad_layout.addWidget(self.dpad_label)
        
        dpad_group.setLayout(dpad_layout)
        parent_layout.addWidget(dpad_group)
        
        # Raw data display
        raw_group = QGroupBox("Raw Button Data (Hex)")
        raw_layout = QVBoxLayout()
        
        raw_info_label = QLabel("Displays raw button bitmap - useful for identifying unmapped buttons")
        raw_info_label.setWordWrap(True)
        raw_layout.addWidget(raw_info_label)
        
        self.raw_data_label = QLabel("0x0000")
        font = QFont("Courier")
        font.setPointSize(14)
        font.setBold(True)
        self.raw_data_label.setFont(font)
        self.raw_data_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        raw_layout.addWidget(self.raw_data_label)
        
        raw_group.setLayout(raw_layout)
        parent_layout.addWidget(raw_group)
    
    def toggle_debug_mode(self):
        """Toggle debug mode on/off"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        self.debug_mode_active = not self.debug_mode_active
        
        if self.debug_mode_active:
            # Start debug mode
            try:
                # Send debug mode command to device
                self.serial_port.write(b"DEBUG_START\n")
                self.debug_timer.start()
                self.debug_start_btn.setText("Stop Debug Mode")
                self.debug_status_label.setText("Status: Active")
                self.statusBar().showMessage("Debug mode started")
            except Exception as e:
                self.debug_mode_active = False
                QMessageBox.critical(self, "Error", f"Failed to start debug mode: {e}")
        else:
            # Stop debug mode
            try:
                self.serial_port.write(b"DEBUG_STOP\n")
                self.debug_timer.stop()
                self.debug_start_btn.setText("Start Debug Mode")
                self.debug_status_label.setText("Status: Not Active")
                self.statusBar().showMessage("Debug mode stopped")
                self.reset_debug_display()
            except Exception as e:
                QMessageBox.warning(self, "Warning", f"Failed to stop debug mode: {e}")
    
    def poll_debug_data(self):
        """Poll for debug data from device"""
        if not self.serial_port or not self.serial_port.is_open:
            return
        
        try:
            # Request debug data
            self.serial_port.write(b"DEBUG_GET\n")
            
            # Wait briefly for response
            time.sleep(0.01)
            
            # Read available data
            if self.serial_port.in_waiting > 0:
                line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                if line.startswith("DEBUG:"):
                    self.parse_debug_data(line[6:])
        except Exception as e:
            # Silently ignore errors during polling to avoid spam
            pass
    
    def parse_debug_data(self, data):
        """Parse debug data string and update display"""
        try:
            # Expected format: "buttons,lx,ly,rx,ry,lt,rt,dx,dy"
            parts = data.split(',')
            if len(parts) >= 9:
                buttons = int(parts[0])
                left_x = int(parts[1])
                left_y = int(parts[2])
                right_x = int(parts[3])
                right_y = int(parts[4])
                left_trigger = int(parts[5])
                right_trigger = int(parts[6])
                dpad_x = int(parts[7])
                dpad_y = int(parts[8])
                
                self.update_debug_display(buttons, left_x, left_y, right_x, right_y,
                                         left_trigger, right_trigger, dpad_x, dpad_y)
        except Exception as e:
            # Ignore parse errors
            pass
    
    def update_debug_display(self, buttons, left_x, left_y, right_x, right_y,
                            left_trigger, right_trigger, dpad_x, dpad_y):
        """Update debug display with gamepad state"""
        # Update button states
        button_map = {
            "A": 0x0001,
            "B": 0x0002,
            "X": 0x0004,
            "Y": 0x0008,
            "LB": 0x0010,
            "RB": 0x0020,
            "Back": 0x0040,
            "Start": 0x0080,
            "LS": 0x0100,
            "RS": 0x0200
        }
        
        for name, mask in button_map.items():
            pressed = (buttons & mask) != 0
            self.button_widgets[name].setChecked(pressed)
            if pressed:
                self.button_widgets[name].setStyleSheet("QCheckBox { color: green; font-weight: bold; }")
            else:
                self.button_widgets[name].setStyleSheet("")
        
        # Update analog sticks
        self.left_x_label.setText(f"X: {left_x}")
        self.left_y_label.setText(f"Y: {left_y}")
        self.left_x_bar.setValue(left_x)
        self.left_y_bar.setValue(left_y)
        
        self.right_x_label.setText(f"X: {right_x}")
        self.right_y_label.setText(f"Y: {right_y}")
        self.right_x_bar.setValue(right_x)
        self.right_y_bar.setValue(right_y)
        
        # Update triggers
        self.left_trigger_label.setText(str(left_trigger))
        self.left_trigger_bar.setValue(left_trigger)
        self.right_trigger_label.setText(str(right_trigger))
        self.right_trigger_bar.setValue(right_trigger)
        
        # Update D-pad
        dpad_text = "Center"
        if dpad_y < 0:
            dpad_text = "Up"
        elif dpad_y > 0:
            dpad_text = "Down"
        
        if dpad_x < 0:
            dpad_text = "Left" if dpad_text == "Center" else dpad_text + "-Left"
        elif dpad_x > 0:
            dpad_text = "Right" if dpad_text == "Center" else dpad_text + "-Right"
        
        self.dpad_label.setText(dpad_text)
        
        # Update raw data
        self.raw_data_label.setText(f"0x{buttons:04X}")
    
    def reset_debug_display(self):
        """Reset all debug display widgets to default state"""
        for checkbox in self.button_widgets.values():
            checkbox.setChecked(False)
            checkbox.setStyleSheet("")
        
        self.left_x_label.setText("X: 0")
        self.left_y_label.setText("Y: 0")
        self.left_x_bar.setValue(0)
        self.left_y_bar.setValue(0)
        
        self.right_x_label.setText("X: 0")
        self.right_y_label.setText("Y: 0")
        self.right_x_bar.setValue(0)
        self.right_y_bar.setValue(0)
        
        self.left_trigger_label.setText("0")
        self.left_trigger_bar.setValue(0)
        self.right_trigger_label.setText("0")
        self.right_trigger_bar.setValue(0)
        
        self.dpad_label.setText("Center")
        self.raw_data_label.setText("0x0000")
    
    def refresh_ports(self):
        """Refresh available serial ports"""
        current_port = self.port_combo.currentText()
        self.port_combo.clear()
        
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_combo.addItem(f"{port.device} - {port.description}")
        
        # Try to restore previous selection
        index = self.port_combo.findText(current_port)
        if index >= 0:
            self.port_combo.setCurrentIndex(index)
    
    def toggle_connection(self):
        """Connect or disconnect from device"""
        if self.serial_port and self.serial_port.is_open:
            self.disconnect_device()
        else:
            self.connect_device()
    
    def connect_device(self):
        """Connect to device"""
        port_text = self.port_combo.currentText()
        if not port_text:
            QMessageBox.warning(self, "Error", "No port selected")
            return
        
        port = port_text.split(" - ")[0]
        
        try:
            self.serial_port = serial.Serial(port, 115200, timeout=1)
            self.connect_button.setText("Disconnect")
            self.statusBar().showMessage(f"Connected to {port}")
        except Exception as e:
            QMessageBox.critical(self, "Connection Error", str(e))
    
    def disconnect_device(self):
        """Disconnect from device"""
        # Stop debug mode if active
        if self.debug_mode_active:
            self.debug_mode_active = False
            self.debug_timer.stop()
            self.reset_debug_display()
        
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
        self.connect_button.setText("Connect")
        self.statusBar().showMessage("Disconnected")
    
    def on_output_changed(self, index):
        """Handle output type change"""
        output_type = ["Gamepad", "Keyboard", "Mouse", "Combo"][index]
        self.statusBar().showMessage(f"Output type changed to {output_type}")
    
    def add_mapping(self):
        """Add button mapping"""
        # TODO: Show dialog to add mapping
        QMessageBox.information(self, "Add Mapping", "Mapping dialog not yet implemented")
    
    def remove_mapping(self):
        """Remove selected button mapping"""
        current_row = self.mapping_table.currentRow()
        if current_row >= 0:
            self.mapping_table.removeRow(current_row)
    
    def add_macro(self):
        """Add new macro"""
        dialog = MacroEditorDialog(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            macro_data = dialog.get_macro_data()
            self.macros.append(macro_data)
            self.update_macro_table()
    
    def edit_macro(self):
        """Edit selected macro"""
        current_row = self.macro_table.currentRow()
        if current_row >= 0 and current_row < len(self.macros):
            macro_data = self.macros[current_row]
            dialog = MacroEditorDialog(self, macro_data)
            if dialog.exec() == QDialog.DialogCode.Accepted:
                self.macros[current_row] = dialog.get_macro_data()
                self.update_macro_table()
    
    def remove_macro(self):
        """Remove selected macro"""
        current_row = self.macro_table.currentRow()
        if current_row >= 0 and current_row < len(self.macros):
            self.macros.pop(current_row)
            self.update_macro_table()
    
    def update_macro_table(self):
        """Update macro table display"""
        self.macro_table.setRowCount(len(self.macros))
        for i, macro in enumerate(self.macros):
            self.macro_table.setItem(i, 0, QTableWidgetItem(str(macro['id'])))
            step_count = len(macro['steps'].split('\n')) if macro['steps'] else 0
            self.macro_table.setItem(i, 1, QTableWidgetItem(f"{step_count} steps"))
    
    def load_config(self):
        """Load configuration from device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        # TODO: Implement config loading protocol
        QMessageBox.information(self, "Load Config", "Config loading not yet implemented")
    
    def save_config(self):
        """Save configuration to device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        # TODO: Implement config saving protocol
        QMessageBox.information(self, "Save Config", "Config saving not yet implemented")
    
    def refresh_logs(self):
        """Fetch logs from device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        try:
            # Clear any pending data
            self.serial_port.reset_input_buffer()
            
            # Request logs
            self.serial_port.write(b"LOG_GET\n")
            
            # Wait for response
            time.sleep(0.1)
            
            # Read response
            logs = []
            in_log_block = False
            timeout_count = 0
            max_timeout = 50  # 5 seconds max
            
            while timeout_count < max_timeout:
                if self.serial_port.in_waiting > 0:
                    line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line == "LOG_START":
                        in_log_block = True
                        continue
                    elif line == "LOG_END":
                        break
                    elif line == "LOG_EMPTY":
                        self.log_text.setPlainText("(No logs available)")
                        self.statusBar().showMessage("No logs on device")
                        return
                    elif in_log_block:
                        logs.append(line)
                    
                    timeout_count = 0  # Reset timeout on data received
                else:
                    time.sleep(0.1)
                    timeout_count += 1
            
            if logs:
                self.log_text.setPlainText("\n".join(logs))
                # Scroll to bottom
                cursor = self.log_text.textCursor()
                cursor.movePosition(QTextCursor.MoveOperation.End)
                self.log_text.setTextCursor(cursor)
                self.statusBar().showMessage(f"Retrieved {len(logs)} log lines")
            else:
                self.log_text.setPlainText("(No logs retrieved)")
                self.statusBar().showMessage("No logs retrieved")
                
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to fetch logs: {e}")
    
    def clear_logs(self):
        """Clear logs on device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        try:
            self.serial_port.write(b"LOG_CLEAR\n")
            time.sleep(0.1)
            
            # Read response
            if self.serial_port.in_waiting > 0:
                response = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                if response == "LOG_CLEARED":
                    self.log_text.clear()
                    self.log_count_label.setText("Entries: 0")
                    self.log_overflow_label.setText("Overflow: No")
                    self.statusBar().showMessage("Logs cleared")
                else:
                    self.statusBar().showMessage(f"Unexpected response: {response}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to clear logs: {e}")
    
    def export_logs(self):
        """Export logs to file"""
        log_content = self.log_text.toPlainText()
        if not log_content or log_content.startswith("("):
            QMessageBox.warning(self, "Warning", "No logs to export")
            return
        
        # Generate default filename with timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        default_filename = f"joystick_converter_logs_{timestamp}.txt"
        
        filename, _ = QFileDialog.getSaveFileName(
            self,
            "Export Logs",
            default_filename,
            "Text Files (*.txt);;All Files (*)"
        )
        
        if filename:
            try:
                with open(filename, 'w', encoding='utf-8') as f:
                    f.write(f"Joystick Converter Device Logs\n")
                    f.write(f"Exported: {datetime.now().isoformat()}\n")
                    f.write("=" * 50 + "\n\n")
                    f.write(log_content)
                self.statusBar().showMessage(f"Logs exported to {filename}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to export logs: {e}")
    
    def set_log_level(self, index):
        """Set device log level"""
        if not self.serial_port or not self.serial_port.is_open:
            return
        
        try:
            command = f"LOG_LEVEL {index}\n"
            self.serial_port.write(command.encode())
            time.sleep(0.1)
            
            if self.serial_port.in_waiting > 0:
                response = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                if response.startswith("LOG_LEVEL_SET:"):
                    level_names = ["DEBUG", "INFO", "WARN", "ERROR"]
                    self.statusBar().showMessage(f"Log level set to {level_names[index]}")
                else:
                    self.statusBar().showMessage(f"Failed to set log level: {response}")
        except Exception as e:
            QMessageBox.warning(self, "Warning", f"Failed to set log level: {e}")
    
    def get_log_status(self):
        """Get log status from device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        try:
            self.serial_port.reset_input_buffer()
            self.serial_port.write(b"LOG_STATUS\n")
            time.sleep(0.1)
            
            if self.serial_port.in_waiting > 0:
                response = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                if response.startswith("LOG_STATUS:"):
                    # Parse: LOG_STATUS:level=X,count=Y,overflow=Z
                    status_str = response[11:]
                    parts = dict(item.split('=') for item in status_str.split(','))
                    
                    count = int(parts.get('count', '0'))
                    overflow = parts.get('overflow', '0') == '1'
                    level = int(parts.get('level', '1'))
                    
                    self.log_count_label.setText(f"Entries: {count}")
                    self.log_overflow_label.setText(f"Overflow: {'Yes' if overflow else 'No'}")
                    self.log_level_combo.setCurrentIndex(level)
                    
                    self.statusBar().showMessage("Log status updated")
                else:
                    self.statusBar().showMessage(f"Unexpected response: {response}")
        except Exception as e:
            QMessageBox.warning(self, "Warning", f"Failed to get log status: {e}")
    
    def closeEvent(self, event):
        """Handle window close"""
        self.disconnect_device()
        event.accept()


def main():
    """Main entry point"""
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Modern look
    
    window = ConfigTool()
    window.show()
    
    sys.exit(app.exec())


if __name__ == '__main__':
    main()
