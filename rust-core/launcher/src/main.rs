use slint::ComponentHandle;
use std::process::Command;

slint::include_modules!();

fn match_app(app: &str) -> &'static str {
    match app {
        "ayarlar" => "mauvyctl",
        "telefon" => "shell",
        "modem" => "patinet",
        "dosyalar" => "shell",
        "tarayici" => "shell",
        "terminal" => "shell",
        "hakkinda" => "uname",
        "kapat" => "halt",
        _ => {
            eprintln!("[LAUNCHER] Bilinmeyen uygulama: {}", app);
            ""
        }
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let ui = LauncherApp::new()?;
    let ui_weak = ui.as_weak();

    ui.set_status_text("Hazır — bir uygulama seçin".into());

    ui.on_launch_app(move |app_name| {
        let app = app_name.to_string();
        let cmd = match_app(&app);
        if cmd.is_empty() {
            return;
        }
        match Command::new(cmd).spawn() {
            Ok(child) => {
                println!("[LAUNCHER] '{}' başlatıldı (pid={:?})", cmd, child.id());
                if let Some(u) = ui_weak.upgrade() {
                    u.set_status_text(format!("{} başlatıldı", app).into());
                }
            }
            Err(e) => {
                eprintln!("[LAUNCHER] '{}' başlatılamadı: {}", cmd, e);
                if let Some(u) = ui_weak.upgrade() {
                    u.set_status_text(format!("{} hata", app).into());
                }
            }
        }
    });

    ui.run()?;
    Ok(())
}
