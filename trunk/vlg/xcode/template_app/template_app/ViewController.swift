//
//  ViewController.swift
//  template_app
//
//  Created by Giuseppe Baccini on 05/08/18.
//  Copyright © 2018 Giuseppe Baccini. All rights reserved.
//

import UIKit

let PEER_VER: [CUnsignedInt] = [0,0,0,0]
let PEER_NAME = CString("peer")

class ViewController: UIViewController {
    
    required init?(coder aDecoder: NSCoder) {
        peer_ = Peer(peer_name: PEER_NAME, peer_ver: PEER_VER)
        super.init(coder: aDecoder)
        peer_.ViewController = self
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func StartPeer(_ sender: UIButton) {
        print("start")
        peer_.StartPeer()
    }
    
    @IBAction func StopPeer(_ sender: UIButton) {
        print("stop")
        peer_.StopPeer()
    }
    
    let peer_ : Peer
    
    @IBOutlet weak var startB_: UIButton!
    @IBOutlet weak var stopB_: UIButton!
    @IBOutlet weak var peerStatus_: UILabel!
}

