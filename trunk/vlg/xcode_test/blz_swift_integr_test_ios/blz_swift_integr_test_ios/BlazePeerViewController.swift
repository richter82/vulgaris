//
//  BlazePeerViewController.swift
//  blz_swift_integr_test_ios
//
//  Created by Giuseppe Baccini on 03/05/15.
//  Copyright (c) 2015 blaze. All rights reserved.
//

import UIKit

let BLZ_SWIFT_TEST_PEER_VER: [CUnsignedInt] = [0,0,0,1]
let BLZ_SWIFT_TEST_PEER_NAME = "mobile_test_peer"

class BlazePeerViewController: UIViewController
{
    @IBOutlet weak var peerStatus: UILabel!
    
    @IBAction func startStopAction(_ sender: UIButton) {
        if(sender.currentTitle == "Start"){
            mobilePeer?.startPeer()
        }else{
            mobilePeer?.stopPeer()
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }

//------------------------------------------------------------------------------
// Internal
//------------------------------------------------------------------------------
    var mobilePeer = MobilePeer.getMobilePeerInstance(BLZ_SWIFT_TEST_PEER_NAME, version: BLZ_SWIFT_TEST_PEER_VER)
}
