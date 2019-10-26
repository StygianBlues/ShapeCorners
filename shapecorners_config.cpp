#include "shapecorners_config.h"
#include "ui_shapecorners_config.h"

#include <QDialog>
#include <QVBoxLayout>

#include <kwineffects.h>
#include <kwineffects_interface.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusArgument>

#include <KConfigGroup>
#include <KPluginFactory>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory,
                           "shapecorners_config.json",
                           registerPlugin<ShapeCornersConfig>();)


class ConfigDialog : public QDialog , public Ui::Dialog
{
public:
    explicit ConfigDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(0);
    setupUi(this);
}
};

class ShapeCornersConfig::Private
{
public:
    Private(ShapeCornersConfig *config)
        : q(config)
        , roundness("roundness")
        , shadows("shadows")
        , defaultRoundness(5)
        , defaultShadows(false)
    {}
    ShapeCornersConfig *q;
    QString roundness, shadows;
    QVariant defaultRoundness, defaultShadows;
    ConfigDialog *ui;
};

ShapeCornersConfig::ShapeCornersConfig(QWidget* parent, const QVariantList& args)
    : KCModule(KAboutData::pluginData(QStringLiteral("kwin4_effect_shapecorners")), parent, args)
    , d(new Private(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(d->ui = new ConfigDialog(this));
    setLayout(layout);
    setMinimumHeight(300);
}

void
ShapeCornersConfig::load()
{
    KCModule::load();
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    d->ui->roundness->setValue(conf.readEntry(d->roundness, d->defaultRoundness).toInt());
    d->ui->shadows->setChecked(conf.readEntry(d->shadows, d->defaultShadows).toBool());
    emit changed(false);
}

void
ShapeCornersConfig::save()
{
    KCModule::save();
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    conf.writeEntry(d->roundness, d->ui->roundness->value());
    conf.writeEntry(d->shadows, d->ui->shadows->isChecked());
    conf.sync();
    emit changed(false);
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_shapecorners"));
}

void
ShapeCornersConfig::defaults()
{
    KCModule::defaults();
    d->ui->roundness->setValue(d->defaultRoundness.toInt());
    d->ui->shadows->setChecked(d->defaultShadows.toBool());
    emit changed(true);
}

#include "shapecorners_config.moc"
