include("defaultConfig.jl")

sloppyfocus = 1
borderpx = 1
rootcolor = [0.3, 0.3, 0.3, 1.0]
bordercolor = [0.3, 0.3, 0.3, 1.0]
focuscolor = [1.0, 0.0, 0.0, 0.0]

tags = [ "1", "2", "3", "4", "5", "6", "7", "8", "9" ]

# where to put things
rules = [
    [ "Gimp", "title", 1, true, 3]
]

layouts = [
    [ "[M]", (n)-> Layouts.monocle(n) ],
    [ "[]=", (n)-> Layouts.tile(n) ],
    [ "><>", (n)-> Layouts.floating(n) ],
]

layout = layouts[1]

monrules = [
    # name mfact nmaster scale layout transform
    [ "rule", 0.55, 1, 1, layouts[1], NORMAL ],
]

xkb_rules = []
repeatRate = 25
repeatDelay = 600
termcmd = "/usr/bin/termite"

mod = mod1
#maps (between 1 and 4)
keys = [
        ["$mod u",           ()->  spawn(termcmd)        ],
        ["$mod period",      ()->  focusmon(+1)           ],
        ["$mod comma",       ()->  focusmon(-1)           ],
        ["$mod k",           ()->  focusstack(-1)         ],
        ["$mod j",           ()->  focusstack(1)          ],
        ["$mod d",           ()->  incnmaster(-1)         ],
        ["$mod c",           ()->  killclient()           ],
        ["$mod q",           ()->  quit()                 ],
        ["$mod p",           ()->  Layouts.splitThisContainer(1/2)                 ],
        ["$mod o",           ()->  Layouts.vsplitThisContainer(1/2)                 ],
        ["$mod i",           ()->  Layouts.mergeContainer(1, 1, 2)                 ],
        ["$mod space",       ()->  setLayout()            ],
        ["$mod m",           ()->  setLayout(1)  ],
        ["$mod t",           ()->  setLayout(2)  ],
        ["$mod f",           ()->  setLayout(3)  ],
        ["$mod l",           ()->  Layouts.resizeThisAll(1/10, Layouts.RIGHT)        ],
        ["$mod h",           ()->  Layouts.resizeThisAll(1/10, Layouts.LEFT)        ],
        ["$mod parenright",  ()->  tag(~0)                ],
        ["$mod greater",     ()->  tagmon(+1)             ],
        ["$mod less",        ()->  tagmon(-1)             ],
        ["$mod Tab",         ()->  view()                 ],
        ["$mod 0",           ()->  view(~0)               ],
        ["$mod Return",      ()->  zoom()                 ],
        ["$mod r",           ()->  reloadConfig()                 ],
        ["$mod s",           ()->  createNewOverlay()                 ],
       ]

buttons = [
           ["$mod $btnLeft",    ()  ->  moveResize(CurMove)      ],
           ["$mod $btnMiddle",  ()  ->  toggleFloating()  ],
           ["$mod $btnRight",   ()  ->  moveResize(CurResize)      ],
          ]
