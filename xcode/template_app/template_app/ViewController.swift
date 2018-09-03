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
        oc_ = OutgoingConnection(peer: peer_)
        super.init(coder: aDecoder)
        peer_.ViewController = self
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        connectOC_.isEnabled = false
        disconnectOC_.isEnabled = false
    }
    
    @IBAction func StartPeer(_ sender: UIButton) {
        peer_.startPeer()
        connectOC_.isEnabled = true
        disconnectOC_.isEnabled = false
    }
    
    @IBAction func StopPeer(_ sender: UIButton) {
        peer_.stopPeer()
        connectOC_.isEnabled = false
        disconnectOC_.isEnabled = false
    }
    
    @IBAction func connectOC(_ sender: UIButton) {
        oc_.connect()
    }
    
    @IBAction func disconnectOC(_ sender: UIButton) {
    }
    
    let peer_: Peer
    let oc_: OutgoingConnection
    
    @IBOutlet weak var startB_: UIButton!
    @IBOutlet weak var stopB_: UIButton!
    @IBOutlet weak var peerStatus_: UILabel!
    
    @IBOutlet weak var connectOC_: UIButton!
    @IBOutlet weak var disconnectOC_: UIButton!
    @IBOutlet weak var ocStatus_: UILabel!
}

