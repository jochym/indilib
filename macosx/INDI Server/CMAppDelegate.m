//
//  CMAppDelegate.m
//  INDI Server
//
//  Created by Peter Polakovic on 12.1.2014.
//  Copyright (c) 2014 CloudMakers, s. r. o. All rights reserved.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License version 2 as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public License
//  along with this library; see the file COPYING.LIB.  If not, write to
//  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
//  Boston, MA 02110-1301, USA.

#include <sys/stat.h>

#import <ServiceManagement/ServiceManagement.h>
#import <Security/Authorization.h>
#import "CMAppDelegate.h"
#import "CMGroup.h"
#import "CMDevice.h"

#define LOGNAME "/Users/%s/Library/Logs/indiserver.log"
#define FIFONAME "/tmp/indiserverFIFO"

@interface CMAppDelegate() {
  NSString *serverId;
  NSBundle *mainBundle;
  NSString *bin;
  NSString *prefix;
  CMGroup *currentGroup;
  CMDevice *currentDevice;
  NSMutableArray *groups;
  NSMutableArray *devices;
  NSMutableString *characters;
  int pipe;
}

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSOutlineView *deviceTree;
@property (weak) IBOutlet NSTableView *deviceList;
@property (weak) IBOutlet NSButton *addButton;
@property (weak) IBOutlet NSButton *removeButton;
@property (weak) IBOutlet NSButton *reloadButton;
@property (weak) IBOutlet NSImageView *statusImage;
@property (weak) IBOutlet NSTextField *statusLabel;
@property (unsafe_unretained) IBOutlet NSPanel *addDeviceSheet;

@end

@implementation CMAppDelegate

// NSObject

-(id) init {
  self = [super init];
  if (self) {
    serverId = @"eu.cloudmakers.indi.indiserver";
    mainBundle = [NSBundle mainBundle];
    bin = [mainBundle pathForResource:@"bin" ofType:nil];
    prefix = [bin stringByDeletingLastPathComponent];
    groups = nil;
    currentGroup = nil;
    currentDevice = nil;
    characters = [NSMutableString stringWithString:@""];
  }
  return self;
}

// NSOutlineViewDataSource

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item {
  if (item == nil) {
    return groups[index];
  }
  if ([item class] == [CMGroup class]) {
    CMGroup *group = item;
    return group.devices[index];
  }
  return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item {
  if (item == nil) {
    return groups.count > 0;
  }
  if ([item class] == [CMGroup class]) {
    CMGroup *group = item;
    return group.devices.count > 0;
  }
  return NO;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item {
  if (item == nil) {
    return groups.count;
  }
  if ([item class] == [CMGroup class]) {
    CMGroup *group = item;
    return group.devices.count;
  }
  return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item {
  if ([tableColumn.identifier isEqualToString:@"name"]) {
    if ([item class] == [CMGroup class]) {
      CMGroup *group = item;
      return group.description;
    }
    if ([item class] == [CMDevice class]) {
      CMDevice *device = item;
      return device.description;
    }
  } else if ([tableColumn.identifier isEqualToString:@"driver"]) {
    if ([item class] == [CMDevice class]) {
      CMDevice *device = item;
      return device.driver;
    }
  }
  return @"";
}

-(BOOL)outlineView:(NSOutlineView*)outlineView isGroupItem:(id)item {
  return [item class] == [CMGroup class];
}

// NSTableViewDelegate, NSTableViewDataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
  return devices.count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
  CMDevice *device = devices[row];
  return device;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
  NSTableCellView *cell = [tableView makeViewWithIdentifier:@"cell" owner:self];
  CMDevice *device = [self tableView:tableView objectValueForTableColumn:tableColumn row:row];
  cell.textField.stringValue = device.name;
  switch (device.status) {
    case STARTING:
      cell.imageView.image = [NSImage imageNamed:@"NSStatusPartiallyAvailable"];
      break;
    case STARTED:
      cell.imageView.image = [NSImage imageNamed:@"NSStatusAvailable"];
      break;
    case FAILED:
      cell.imageView.image = [NSImage imageNamed:@"NSStatusUnavailable"];
      break;
    default:
      cell.imageView.image = [NSImage imageNamed:@"NSStatusNone"];
      break;
  }
  return cell;
}

// NSXMLParserDelegate

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict {
  [characters setString:@""];
  if ([elementName isEqualToString:@"devGroup"]) {
    NSString *name = attributeDict[@"group"];
    currentGroup = nil;
    for (CMGroup *group in groups) {
      if ([group.name isEqualToString:name]) {
        currentGroup = group;
        break;
      }
    }
    if (!currentGroup) {
      currentGroup = [[CMGroup alloc] init];
      currentGroup.name = name;
      currentGroup.devices = [NSMutableArray array];
    }
    return;
  }
  if ([elementName isEqualToString:@"device"]) {
    NSString *name = attributeDict[@"label"];
    currentDevice = [[CMDevice alloc] init];
    currentDevice.name = name;
    return;
  }
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string {
  [characters appendString:[string stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]]];
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName {
  if ([elementName isEqualToString:@"driver"]) {
    NSString *path = [mainBundle pathForResource:characters ofType:nil inDirectory:@"bin"];
    if (path) {
      currentDevice.driver = [characters copy];
      if (![groups containsObject:currentGroup]) {
        [groups addObject:currentGroup];
      }
      if (![currentGroup.devices containsObject:currentDevice]) {
        [currentGroup.devices addObject:currentDevice];
      }
    }
    return;
  }
  if ([elementName isEqualToString:@"version"]) {
    currentDevice.version = [characters copy];
    return;
  }
}
  
- (void)parserDidEndDocument:(NSXMLParser *)parser {
}

- (void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError {
  NSLog(@"parseErrorOccurred %@", parseError);
}

// NSApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  [self startServer];
  NSError *error = nil;
  NSMutableString *root = [NSMutableString stringWithString:@"<?xml version='1.0' encoding='UTF-8'?>"];
  [root appendString:@"<root>"];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"drivers" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_atik" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_eqmod" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_qhy" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_sx" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_shoestring" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_maxdomeii" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:[NSString stringWithContentsOfFile:[mainBundle pathForResource: @"indi_spectracyber" ofType: @"xml" inDirectory:@"share/indi"] encoding:NSUTF8StringEncoding error:&error]];
  [root appendString:@"</root>"];
  groups = [NSMutableArray array];
  NSXMLParser *parser = [[NSXMLParser alloc] initWithData:[root dataUsingEncoding:NSUTF8StringEncoding]];
  [parser setDelegate:self];
  [parser setShouldProcessNamespaces:NO];
  [parser setShouldReportNamespacePrefixes:NO];
  [parser setShouldResolveExternalEntities:YES];
  [parser parse];
  [_deviceTree reloadData];
  devices = [NSMutableArray array];
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  NSData *data = [defaults objectForKey:@"devices"];
  if (data) {
    NSArray *savedDevices = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    for (CMDevice *savedDevice in savedDevices) {
      for (CMGroup *group in groups) {
        for (CMDevice *device in group.devices) {
          if ([device.name isEqualToString:savedDevice.name]) {
            @synchronized(devices) {
              [devices addObject:device];
            }
            [self startDriver:device];
          }
        }
      }
    }
  }
  [_deviceList reloadData];
  [_removeButton setEnabled:NO];
  [_reloadButton setEnabled:NO];
}

-(void) applicationWillTerminate:(NSNotification *)notification {
  [self stopServer];
}

// CMAppDelegate

-(void) setStatus:(CMStatus)status to:(NSString *)driver {
  @synchronized(devices) {
    for (CMDevice *device in devices) {
      if ([device.driver isEqualToString:driver]) {
        device.status = status;
        [_deviceList performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
        return;
      }
    }
  }
  NSLog(@"%@ not found in %@", driver, devices);
}

-(void) logReader:(NSString *)logname {
  char buffer[1024];
  char driver[128];
  int count;
  FILE *log = NULL;
  long pos;
  NSLog(@"logReader started");
  while (!log) {
    sleep(1);
    log = fopen([logname cStringUsingEncoding:NSASCIIStringEncoding], "r");
  }
  _statusImage.image = [NSImage imageNamed:@"NSStatusAvailable"];
  _statusLabel.stringValue = @"Server is running (idle)";
  while (true) {
    pos = ftell(log);
    while (!fgets(buffer, 1024, log)) {
      sleep(1);
      fseek(log, pos, SEEK_SET);
    }
    if (sscanf(buffer, "STARTING \"%128[^\"]\"", driver) == 1) {
      NSLog(@"starting %s", driver);
      [self setStatus:STARTING to:[NSString stringWithCString:driver encoding:NSASCIIStringEncoding]];
    } else if (sscanf(buffer, "STARTED \"%128[^\"]\"", driver) == 1) {
      NSLog(@"started %s", driver);
      [self setStatus:STARTED to:[NSString stringWithCString:driver encoding:NSASCIIStringEncoding]];
    } else if (sscanf(buffer, "FAILED \"%128[^\"]\"", driver) == 1) {
      NSLog(@"failed %s", driver);
      [self setStatus:FAILED to:[NSString stringWithCString:driver encoding:NSASCIIStringEncoding]];
    } else if (sscanf(buffer, "CLIENTS %d", &count) == 1) {
      NSLog(@"count %d", count);
      switch (count) {
        case 0:
          _statusLabel.stringValue = @"Server is running (idle)";
          break;
        case 1:
          _statusLabel.stringValue = @"Server is running (1 client)";
          break;
        default:
          _statusLabel.stringValue = [NSString stringWithFormat:@"Server is running (%d clients)", count];
          break;
      }
    }
  }
}

-(void) startServer {
  _statusImage.image = [NSImage imageNamed:@"NSStatusPartiallyAvailable"];
  _statusLabel.stringValue = @"Clean up...";
  char logname[1024];
  snprintf(logname, 1024, LOGNAME, getlogin());
  unlink(logname);
  unlink(FIFONAME);
  mode_t cmask = umask(0);
  mkfifo(FIFONAME, 0666);
  umask(cmask);
  pipe = open(FIFONAME, O_RDWR);
  
  _statusLabel.stringValue = @"Installing server job...";
  SMJobRemove(kSMDomainUserLaunchd, (__bridge CFStringRef)serverId, nil, false, NULL);
  NSMutableDictionary *plist = [NSMutableDictionary dictionary];
  [plist setObject:serverId forKey:@"Label"];
  [plist setObject:[NSNumber numberWithBool:YES] forKey:@"KeepAlive"];
  [plist setObject:[mainBundle pathForAuxiliaryExecutable:@"indiserver"] forKey:@"Program"];
  CFErrorRef error;
  if (SMJobSubmit(kSMDomainUserLaunchd, (__bridge CFDictionaryRef)plist, nil, &error)) {
  } else {
    _statusImage.image = [NSImage imageNamed:@"NSStatusUnavailable"];
    _statusLabel.stringValue = @"Server job failed!";
    NSLog(@"Server job failed %@", error);
  }
  if ( error ) {
    CFRelease(error);
  }
  [[[NSThread alloc] initWithTarget:self selector:@selector(logReader:) object:[NSString stringWithCString:logname encoding:NSASCIIStringEncoding]] start];
}

-(void) stopServer {
  SMJobRemove(kSMDomainUserLaunchd, (__bridge CFStringRef)serverId, nil, false, NULL);
  unlink(FIFONAME);
}

- (IBAction) showAddSheet:(id)sender {
  [NSApp beginSheet:_addDeviceSheet modalForWindow:(NSWindow *)_window modalDelegate:self didEndSelector:nil contextInfo:nil];
}

-(IBAction)removeDriver:(id)sender {
  long row = _deviceList.selectedRow;
  CMDevice *item = devices[row];
  [self stopDriver:item];
  @synchronized(devices) {
    [devices removeObject:item];
  }
  [_deviceList reloadData];
  [self save];
  if (_deviceList.selectedRow >= 0) {
    [_removeButton setEnabled:NO];
    [_reloadButton setEnabled:NO];
  }
}
  
-(IBAction)addDriver:(id)sender {
  long row = _deviceTree.selectedRow;
  CMDevice *item = [_deviceTree itemAtRow:row];
  if ([item class] == [CMDevice class] && ![devices containsObject:item]) {
    [self startDriver:item];
    @synchronized(devices) {
      [devices addObject:item];
    }
    [_deviceList reloadData];
    [self save];
  }
  [NSApp endSheet:_addDeviceSheet];
  [_addDeviceSheet orderOut:sender];
}

-(IBAction)reloadDriver:(id)sender {
  long row = _deviceList.selectedRow;
  CMDevice *device = devices[row];
  [self stopDriver:device];
  [self startDriver:device];
}

-(IBAction)cancel:(id)sender {
  [NSApp endSheet:_addDeviceSheet];
  [_addDeviceSheet orderOut:sender];
}

-(IBAction)click:(id)sender {
  [_removeButton setEnabled:YES];
  [_reloadButton setEnabled:YES];
}

- (IBAction)help:(id)sender {
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.cloudmakers.eu/indi"]];
}

-(void)startDriver:(CMDevice *)device {
  NSString *command = [NSString stringWithFormat:@"start %@ -p \"%@\"\n", device.driver, prefix];
  write(pipe, [command cStringUsingEncoding:NSASCIIStringEncoding], command.length);
}
  
-(void)stopDriver:(CMDevice *)device {
  NSString *command = [NSString stringWithFormat:@"stop %@\n", device.driver];
  write(pipe, [command cStringUsingEncoding:NSASCIIStringEncoding], command.length);
}
  
- (void) save {
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  NSData *data = [NSKeyedArchiver archivedDataWithRootObject:devices];
  [defaults setObject:data forKey:@"devices"];
  [defaults synchronize];
}

@end
