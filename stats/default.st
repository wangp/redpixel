#
# Red Pixel stats file -- ultra fast EVERYTHING version
#
# What's new since stats.09?
#  - approx. 1/3 times faster weapons; 1/3 times more ammo
#  - less max health (200 -> 100) and armour (300 -> 150)
#  - coke gives more (15 -> 25), armour gives less (100 -> 75)
#  - coke, chicken, armour take longer to respawn
#  - weaker bricks but respawn faster
#
# Make sure the remote machine runs with EXACTLY the same stats,
# or your game will go totally out of sync.
#


#
# Weapons
#

knife_anim	2	knife_reload	10	knife_damage	15
pistol_anim	2	pistol_reload	8	pistol_damage	20
bow_anim	2	bow_reload      14	bow_damage	50
shotgun_anim	3	shotgun_reload  12	shotgun_damage	10
uzi_anim	2	uzi_reload	4	uzi_damage	20
m16_anim	1	m16_reload	3	m16_damage	22
minigun_anim	1	minigun_reload	0	minigun_damage	10
bazooka_anim	3	bazooka_reload	20	bazooka_damage	150
			mine_reload	12	mine_damage	60


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

bullet_give	50	bullet_respawn	30
shell_give	12	shell_respawn	20
mine_give	1	mine_respawn	90
arrow_give	10	arrow_respawn	50
rocket_give	5	rocket_respawn	70

pistol_give	20	pistol_respawn	60
shotgun_give	8	shotgun_respawn	75
uzi_give	40	uzi_respawn	90
m16_give	60	m16_respawn	105
minigun_give	80	minigun_respawn	150
bow_give	6	bow_respawn	130
bazooka_give	3	bazooka_respawn 150


#
# Powerups
#

bloodlust_timeout	30	bloodlust_respawn	180
goggles_timeout		60	goggles_respawn 	180
scanner_timeout		60	scanner_respawn		120


#
# Tiles
#

tile_health	15
tile_respawn	10
barrel_damage	50
