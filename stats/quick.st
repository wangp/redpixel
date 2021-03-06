#
# Red Pixel stats file -- even faster version!
#
# Differences between `stats' and this?
#  - faster weapons, except for minigun and bazooka
#  - most weapons less powerful, except for minigun and bazooka
#  - 1/2 times more ammo
#  - 3x more mines, but less damage so it doesn't kill lots of people, just
#    annoying
#  - longer bloodlust and slower respawn
#  - shorter light-amp and quicker respawn
#  - shorter scanner and slower respawn
#  - even weaker bricks and respawns even faster
#  - barrel doesn't do as much
#
#
#   NOTE: Please don't abuse the stats file feature by giving too much ammo,
#	  weapons which are too fast and stuff like that.  It just might
#	  crash your machine (like it did with mine).
#
#
# Make sure the remote machine runs with EXACTLY the same stats,
# or your game will go totally out of sync.
#


#
# Weapons
#

knife_anim	2	knife_reload	8	knife_damage	13
pistol_anim	2	pistol_reload	6	pistol_damage	18
bow_anim	2	bow_reload      8	bow_damage	40
shotgun_anim	3	shotgun_reload  10	shotgun_damage	10
uzi_anim	2	uzi_reload	3	uzi_damage	18
m16_anim	1	m16_reload	2	m16_damage	20
minigun_anim	1	minigun_reload	1	minigun_damage	20
bazooka_anim	3	bazooka_reload	30	bazooka_damage	200
			mine_reload	8	mine_damage	5


#
# Health Food
#

max_health	100
max_armour	150
armour_give	75	armour_respawn	200
chicken_heal	50	chicken_respawn	150
coke_heal	25	coke_respawn	100


# 
# Ammo & pickups
#

bullet_give	75	bullet_respawn	30
shell_give	18	shell_respawn	20
mine_give	3	mine_respawn	80
arrow_give	15	arrow_respawn	50
rocket_give	7	rocket_respawn	70

pistol_give	30	pistol_respawn	60
shotgun_give	12	shotgun_respawn	75
uzi_give	60	uzi_respawn	90
m16_give	90	m16_respawn	105
minigun_give	120	minigun_respawn	150
bow_give	9	bow_respawn	130
bazooka_give	5	bazooka_respawn 150


#
# Powerups
#

bloodlust_timeout	45	bloodlust_respawn	210
goggles_timeout		30	goggles_respawn 	150
scanner_timeout		45	scanner_respawn		150


#
# Tiles
#

tile_health	5
tile_respawn	7
barrel_damage	40
