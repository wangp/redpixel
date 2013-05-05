set(REDPIXEL_SOURCES
    src/backpack.c
    src/blast.c
    src/blod.c
    src/bullet.c
    src/corpse.c
    src/cpu.c
    src/credits.c
    src/demintro.c
    src/demo.c
    src/engine.c
    src/explo.c
    src/fblit.c
    src/gameloop.c
    src/getopt.c
    src/globals.c
    src/inp_demo.c
    src/inp_peer.c
    src/intro.c
    src/launch.c
    src/main.c
    src/map.c
    src/mapper.c
    src/menu.c
    src/message.c
    src/mine.c
    src/mousespr.c
    src/music.c
    src/options.c
    src/particle.c
    src/player.c
    src/plupdate.c
    src/resource.c
    src/rg_rand.c
    src/rnd.c
    src/rpagup.c
    src/rpjgmod.c
    src/rpstring.c
    src/setweaps.c
    src/skhelp.c
    src/sound.c
    src/statlist.c
    src/stats.c
    src/suicide.c
    src/tiles.c
    src/vector.c
    src/vidmode.c
    src/weapon.c

    src/sk/sk.c
    # src/sk/skdummy.c
    src/sk/sklibnet.c

    src/fastsqrt/fastsqrt.c
    )

set(REDPIXEL_INCLUDE_DIRS
    src/include
    src/sk
    )

set(ALLEGRO4_to_5_SOURCES
    allegro4/allegro.c
    allegro4/clip3df.c
    allegro4/color.c
    allegro4/datafile.c
    allegro4/dataregi.c
    allegro4/file.c
    allegro4/font.c
    allegro4/fsel.c
    allegro4/gui.c
    allegro4/guiproc.c
    allegro4/lzss.c
    allegro4/math.c
    allegro4/math3d.c
    allegro4/palette.c
    allegro4/quat.c
    allegro4/sound.c
    allegro4/ufile.c
    allegro4/unicode.c
    )

set(ALLEGRO4_to_5_INCLUDE_DIRS allegro4/include)

set(AGUP_SOURCES
    agup/agtk.c
    agup/agup.c
    agup/awin95.c
    )

set(AGUP_INCLUDE_DIRS agup)

set(LIBNET_SOURCES
    libnet/lib/core/address.c
    libnet/lib/core/channels.c
    libnet/lib/core/classes.c
    libnet/lib/core/config.c
    libnet/lib/core/connhelp.c
    libnet/lib/core/conns.c
    libnet/lib/core/core.c
    libnet/lib/core/drivers.c
    libnet/lib/core/fixaddr.c
    libnet/lib/core/lists.c
    libnet/lib/core/threads.c
    libnet/lib/core/timer.c
    libnet/lib/core/wait.c
    libnet/lib/drivers/inetaddr.c
    libnet/lib/drivers/internet.c
    libnet/lib/drivers/ipx.c
    libnet/lib/drivers/ipxsocks.c
    libnet/lib/drivers/local.c
    libnet/lib/drivers/nonet.c
    # libnet/lib/drivers/serdos.c
    libnet/lib/drivers/serial.c
    libnet/lib/drivers/serlinux.c
    # libnet/lib/drivers/template.c
    libnet/lib/drivers/wsockdos.c
    )

set(LIBNET_INCLUDE_DIRS
    libnet/include
    libnet/lib/include
    )

set(JGMOD_SOURCES
    jgmod/src/file_io.c
    # jgmod/src/jgm.c
    # jgmod/src/jgmod.c
    jgmod/src/load_it.c
    jgmod/src/load_jgm.c
    jgmod/src/load_mod.c
    jgmod/src/load_s3m.c
    jgmod/src/load_xm.c
    jgmod/src/mod.c
    jgmod/src/player.c
    jgmod/src/player2.c
    jgmod/src/player3.c
    jgmod/src/player4.c
    jgmod/src/save_jgm.c
    )

set(JGMOD_INCLUDE_DIRS jgmod/src)

# vim: set sts=4 sw=4 et:
