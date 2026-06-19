#pragma once
#include <QString>

inline QString appStyleSheet() {
    return R"(
QWidget {
    background-color: #0d1117;
    color: #e6edf3;
    font-family: "JetBrains Mono", "Fira Code", monospace;
    font-size: 12px;
}
QMainWindow, QDialog {
    background-color: #0d1117;
}
/* ── Sidebar / panels ── */
QFrame#sidebar, QFrame#panel {
    background-color: #161b22;
    border-right: 1px solid #21262d;
}
QFrame#rightPanel {
    background-color: #161b22;
    border-left: 1px solid #21262d;
}
/* ── Toolbar ── */
QToolBar {
    background-color: #161b22;
    border-bottom: 1px solid #21262d;
    spacing: 6px;
    padding: 4px 8px;
}
/* ── Buttons ── */
QPushButton {
    background-color: #21262d;
    color: #8b949e;
    border: 1px solid #30363d;
    border-radius: 5px;
    padding: 5px 12px;
    min-height: 24px;
}
QPushButton:hover  { background-color: #30363d; color: #e6edf3; }
QPushButton:pressed{ background-color: #1c2128; }
QPushButton#btnPrimary {
    background-color: #1f6feb;
    color: #ffffff;
    border-color: #1f6feb;
}
QPushButton#btnPrimary:hover  { background-color: #388bfd; }
QPushButton#btnDanger  { color: #da3633; border-color: #50201e; }
QPushButton#btnDanger:hover   { background-color: #21262d; }
/* ── Inputs ── */
QLineEdit, QSpinBox, QComboBox {
    background-color: #0d1117;
    border: 1px solid #30363d;
    border-radius: 5px;
    padding: 5px 8px;
    color: #e6edf3;
    selection-background-color: #1f6feb;
}
QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
    border-color: #1f6feb;
}
QComboBox::drop-down { border: none; }
QComboBox QAbstractItemView {
    background-color: #161b22;
    border: 1px solid #30363d;
    selection-background-color: #1f6feb30;
}
/* ── Tables ── */
QTableWidget, QTreeWidget, QListWidget {
    background-color: #0d1117;
    border: none;
    gridline-color: #161b22;
    alternate-background-color: #0d1117;
    selection-background-color: #1f6feb18;
    selection-color: #e6edf3;
    outline: none;
}
QTableWidget::item, QTreeWidget::item, QListWidget::item {
    padding: 6px 10px;
    border-bottom: 1px solid #161b22;
}
QHeaderView::section {
    background-color: #161b22;
    color: #484f58;
    border: none;
    border-bottom: 1px solid #21262d;
    padding: 6px 10px;
    font-size: 10px;
    text-transform: uppercase;
}
/* ── Progress bar ── */
QProgressBar {
    background-color: #21262d;
    border: none;
    border-radius: 2px;
    height: 4px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background-color: #58a6ff;
    border-radius: 2px;
}
QProgressBar[status="done"]::chunk   { background-color: #3fb950; }
QProgressBar[status="failed"]::chunk { background-color: #da3633; }
/* ── Scroll bars ── */
QScrollBar:vertical {
    background: #0d1117; width: 8px; margin: 0;
}
QScrollBar::handle:vertical {
    background: #30363d; border-radius: 4px; min-height: 20px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background: #0d1117; height: 8px; margin: 0;
}
QScrollBar::handle:horizontal {
    background: #30363d; border-radius: 4px;
}
/* ── Status bar ── */
QStatusBar {
    background-color: #1f6feb;
    color: #ffffff;
    font-size: 11px;
}
QStatusBar QLabel { color: #ffffff; padding: 0 6px; }
/* ── Tab bar ── */
QTabBar::tab {
    background: #161b22;
    color: #8b949e;
    padding: 7px 18px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected {
    color: #58a6ff;
    border-bottom: 2px solid #58a6ff;
    background: #0d1117;
}
QTabBar::tab:hover { color: #e6edf3; }
QTabWidget::pane { border: none; }
/* ── Group box ── */
QGroupBox {
    border: 1px solid #21262d;
    border-radius: 6px;
    margin-top: 8px;
    padding-top: 8px;
    font-size: 11px;
    color: #8b949e;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
/* ── Labels ── */
QLabel#labelMuted  { color: #8b949e; }
QLabel#labelGreen  { color: #3fb950; }
QLabel#labelBlue   { color: #58a6ff; }
QLabel#labelRed    { color: #da3633; }
QLabel#labelHash   { color: #3fb950; font-size: 10px; font-family: monospace; }
/* ── Splitter ── */
QSplitter::handle { background: #21262d; width: 1px; height: 1px; }
    )";
}
